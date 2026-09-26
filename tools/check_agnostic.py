#!/usr/bin/env python3
"""The translator, runtime, frontend, generic tools, and engine tests must
be game-agnostic: nothing from one game's symbols, addresses, or quirks.
Everything game-specific lives under game/<id>/.

Fails if any file under the checked directories contains:
- a function name listed in any game/*/funcs.toml or unresolved.toml,
- a function address from those files, written as $XXXXXX or 0xXXXXXX,
- a word from a game's `agnostic_words` list in its game.toml (title,
  characters, source projects, ...).

usage: check_agnostic.py [repo root]
"""
from __future__ import annotations

import os
import re
import sys
import tomllib

CHECKED = ['recomp', 'runtime', 'frontend', 'test', 'tools']
SKIP_FILES = {os.path.join('tools', 'check_agnostic.py')}   # this file names the rule, not a game
EXTS = ('.py', '.c', '.h', '.sh', '.txt', '.md', '.toml')


def game_terms(root: str) -> tuple[set[str], set[int], set[str]]:
    names, addrs, words = set(), set(), set()
    gdir = os.path.join(root, 'game')
    for g in sorted(os.listdir(gdir)) if os.path.isdir(gdir) else []:
        base = os.path.join(gdir, g)
        for fname, key in (('funcs.toml', 'func'), ('unresolved.toml', 'unresolved')):
            path = os.path.join(base, fname)
            if not os.path.isfile(path):
                continue
            with open(path, 'rb') as f:
                t = tomllib.load(f)
            for e in t.get(key, []) + t.get('extern', []):
                names.add(e['name'])
                addrs.add(e['addr'])
        gt = os.path.join(base, 'game.toml')
        if os.path.isfile(gt):
            with open(gt, 'rb') as f:
                words |= {w.lower() for w in tomllib.load(f).get('agnostic_words', [])}
    return names, addrs, words


def main() -> int:
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))
    names, addrs, words = game_terms(root)
    name_re = re.compile(r'\b(' + '|'.join(sorted(map(re.escape, names), key=len, reverse=True))
                         + r')\b') if names else None
    addr_re = re.compile(r'(?:\$|0x)([0-9A-Fa-f]{6})\b')
    word_re = re.compile(r'\b(' + '|'.join(sorted(map(re.escape, words), key=len, reverse=True))
                         + r')\b', re.I) if words else None
    hits = []
    for d in CHECKED:
        for dirpath, _, files in os.walk(os.path.join(root, d)):
            for fn in sorted(files):
                path = os.path.join(dirpath, fn)
                rel = os.path.relpath(path, root)
                if rel in SKIP_FILES or not fn.endswith(EXTS) and fn not in ('pre-commit',):
                    continue
                with open(path, encoding='utf-8', errors='replace') as f:
                    for n, line in enumerate(f, 1):
                        found = []
                        if name_re:
                            found += [m.group(1) for m in name_re.finditer(line)]
                        found += [m.group(0) for m in addr_re.finditer(line)
                                  if int(m.group(1), 16) in addrs]
                        if word_re:
                            found += [m.group(1) for m in word_re.finditer(line)]
                        if found:
                            hits.append(f'{rel}:{n}: {", ".join(sorted(set(found)))}')
    for h in hits:
        print(h)
    print(f'check_agnostic: {len(names)} names, {len(addrs)} addresses, {len(words)} words; '
          f'{len(hits)} hits')
    return 1 if hits else 0


if __name__ == '__main__':
    sys.exit(main())
