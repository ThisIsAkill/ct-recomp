#!/usr/bin/env python3
"""recomp/funcs.py reads entry provenance from funcs.toml: source =
"profile" (an entry state observed at run time) and profile_states (states
added that way), and rejects anything else."""
import os
import sys
import tempfile

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'recomp'))
import decode  # noqa: E402
import funcs  # noqa: E402

fails = 0


def check(ok, what):
    global fails
    if not ok:
        fails += 1
        print('FAIL', what)


def load(extra: str):
    with tempfile.NamedTemporaryFile('w', suffix='.toml', delete=False) as f:
        f.write('[[func]]\nname = "A"\naddr = 0xE48000\nstates = ["m1x0", "m0x0"]\ne = 0\n'
                'module = "banke4"\n' + extra)
    try:
        return funcs.load(f.name)
    finally:
        os.remove(f.name)


fm = load('')[0]
check(fm.source is None and fm.profile_states == (), 'plain entry: no provenance')
fm = load('source = "profile"\nprofile_states = ["m0x0"]\n')[0]
check(fm.source == 'profile' and fm.profile_states == ('m0x0',), f'profile entry: {fm}')
for bad in ('source = "guess"\n', 'profile_states = ["m1x1"]\n'):
    try:
        load(bad)
        check(False, f'rejected: {bad.strip()}')
    except decode.DecodeError:
        pass

print(f"funcs_meta: {'ok' if not fails else str(fails) + ' failed'}")
sys.exit(1 if fails else 0)
