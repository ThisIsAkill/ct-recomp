#!/usr/bin/env python3
"""Label files (ChronoRET asm, second disassembly) are read as UTF-8:
they carry non-ASCII comments, and opening them with the locale encoding
crashes on Windows (cp1252 has undefined bytes). Writes a synthetic bank
file with UTF-8 comment bytes and checks both label parsers read it."""
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'tools'))
import discover  # noqa: E402
import import_chronoret  # noqa: E402


def main() -> int:
    text = ('; café — byte-exact labels\n'
            'org $C01000\n'
            'MyLabel:\n'
            '    RTS\n')
    fails = 0
    with tempfile.TemporaryDirectory() as tmp:
        bank = os.path.join(tmp, 'asm', 'bankC0')
        os.makedirs(bank)
        path = os.path.join(bank, 'bankC0.asm')
        with open(path, 'wb') as f:
            f.write(text.encode('utf-8'))
        os.environ['CHRONORET'] = tmp
        os.environ['CT_DISASM'] = os.path.join(tmp, 'no-disasm')
        names = discover.label_names()
        if names.get(0xC01000) != 'MyLabel':
            print(f'FAIL label_names: got {names.get(0xC01000)}, want MyLabel')
            fails += 1
        entries = import_chronoret.parse(path)
        if not entries or entries[0]['name'] != 'MyLabel':
            print(f'FAIL import_chronoret.parse: got {entries}, want MyLabel first')
            fails += 1
    print('test_label_encoding: ' + ('ok' if not fails else f'{fails} failed'))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
