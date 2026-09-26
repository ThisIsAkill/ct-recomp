#!/usr/bin/env python3
"""tools/sync_symbols.py must not lose entries and must be deterministic.

Every run works on scratch copies of funcs.toml and unresolved.toml (via
--funcs-toml/--unresolved-toml, which the tool both reads and writes), never
the real ones:

1. No dropped entries: re-syncing a copy of the committed funcs.toml keeps
   every [[func]] it already had (by name and address), including the ones
   inside the bank's own auto block.
2. Determinism: syncing two independent copies gives byte-identical output.
3. Idempotency: syncing an already-synced copy again leaves it unchanged.

Skips (exit 77) if DSCOTTON_DIR isn't set, since that's an external
checkout not every environment has.
"""
import os
import shutil
import subprocess
import sys
import tempfile
import tomllib

GAME = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))   # game/ct
REPO = os.path.dirname(os.path.dirname(GAME))
SYNC = os.path.join(GAME, 'tools', 'sync_symbols.py')
BANK = 'C0'


def run_sync(funcs_toml: str, unresolved_toml: str) -> None:
    subprocess.run(
        [sys.executable, SYNC, BANK, '--funcs-toml', funcs_toml, '--unresolved-toml', unresolved_toml],
        check=True, cwd=REPO, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def read(path: str) -> str:
    if not os.path.isfile(path):
        return ''
    with open(path, encoding='utf-8') as f:
        return f.read()


def entries(path: str) -> set[tuple[str, int]]:
    with open(path, 'rb') as f:
        return {(fn['name'], fn['addr']) for fn in tomllib.load(f).get('func', [])}


def main() -> int:
    if not os.environ.get('DSCOTTON_DIR'):
        print('skip: DSCOTTON_DIR not set')
        return 77

    with tempfile.TemporaryDirectory() as d:
        funcs_src = os.path.join(GAME, 'funcs.toml')
        unres_src = os.path.join(GAME, 'unresolved.toml')
        a_funcs, a_unres = os.path.join(d, 'a_funcs.toml'), os.path.join(d, 'a_unresolved.toml')
        b_funcs, b_unres = os.path.join(d, 'b_funcs.toml'), os.path.join(d, 'b_unresolved.toml')
        for dst in (a_funcs, b_funcs):
            shutil.copy(funcs_src, dst)
        for dst in (a_unres, b_unres):
            shutil.copy(unres_src, dst)

        run_sync(a_funcs, a_unres)
        run_sync(b_funcs, b_unres)

        lost = entries(funcs_src) - entries(a_funcs)
        if lost:
            print(f'FAIL: re-sync dropped {len(lost)} existing funcs.toml entries, e.g.:')
            for name, addr in sorted(lost, key=lambda e: e[1])[:10]:
                print(f'  ${addr:06X} {name}')
            return 1

        a_funcs_text, b_funcs_text = read(a_funcs), read(b_funcs)
        a_unres_text, b_unres_text = read(a_unres), read(b_unres)
        if a_funcs_text != b_funcs_text:
            print('FAIL: two independent syncs produced different funcs.toml output')
            return 1
        if a_unres_text != b_unres_text:
            print('FAIL: two independent syncs produced different unresolved.toml output')
            return 1

        before_funcs, before_unres = a_funcs_text, a_unres_text
        run_sync(a_funcs, a_unres)   # re-sync an already-synced copy
        if read(a_funcs) != before_funcs:
            print('FAIL: re-syncing an already-synced funcs.toml changed it (not idempotent)')
            return 1
        if read(a_unres) != before_unres:
            print('FAIL: re-syncing an already-synced unresolved.toml changed it (not idempotent)')
            return 1

    print('ok: sync_symbols.py keeps existing entries, deterministic and idempotent for bank', BANK)
    return 0


if __name__ == '__main__':
    sys.exit(main())
