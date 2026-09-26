#!/usr/bin/env bash
# Repeatedly run discovery; record the first missing jump table when the
# first-target bound agrees with the second disassembly (listed pointers or
# room before the next code). Stops on disagreement for manual review.
set -u
cd "$(dirname "$0")/.."
for round in $(seq 1 ${1:-20}); do
  site=$(python3 tools/discover.py "${@:2}" 2>&1 | grep -o '\$[0-9A-F]\{6\}: J[MS][PR] (\$[0-9A-F]\{4\},X): no jumptable' | head -1 | cut -c2-7)
  [ -z "$site" ] && { echo "no missing tables"; exit 0; }
  # Table already recorded for another site: reuse its count.
  known=$(python3 - "$site" <<'PY'
import sys
sys.path.insert(0, 'recomp')
import decode, funcs
rom = decode.load_rom()
site = int(sys.argv[1], 16)
def table(s):
    i = decode.decode_insn(rom, s, decode.State(True, False))
    return (s & 0xFF0000) | i.operand
t = table(site)
for s, n in funcs.load_jumptables().items():
    if s != site and table(s) == t:
        print(f'{n} {s:06X} {t:06X}')
        break
PY
)
  if [ -n "$known" ]; then
    set -- $known "${@}"
    printf '
# Same table ($%s) as site $%s.
[[jumptable]]
site = 0x%s
count = %s
' "$3" "$2" "$site" "$1" >> funcs.toml
    echo "site $site reuses table \$$3 ($1 entries)"
    shift 3
    continue
  fi
  out=$(python3 tools/table_extent.py "$site")
  tb=$(echo "$out" | sed -n 's/.*-> at most \([0-9]*\) entries/\1/p')
  items=$(echo "$out" | sed -n 's/^pointer items at table label: //p')
  room=$(echo "$out" | sed -n 's/.*(\([0-9]*\) words of room)/\1/p')
  tbl=$(echo "$out" | sed -n 's/^table: \$\(.*\)/\1/p')
  echo "site $site table $tbl bound ${tb:-?} items ${items:-?} room ${room:-?}"
  # Accept only when the sources agree: listed pointers (if any) must equal
  # min(bound, room); otherwise bound and room must be equal.
  ok=0; cnt=""
  if [ -n "$tb" ] && [ -n "$room" ]; then
    lo=$(( tb < room ? tb : room ))
    if [ "${items:-0}" -gt 0 ]; then
      [ "$items" = "$lo" ] && ok=1 && cnt=$items
    else
      [ "$tb" = "$room" ] && ok=1 && cnt=$tb
    fi
  fi
  tb=$cnt
  if [ "$ok" = 1 ]; then
    printf '\n# index not bounded in code; first target after $%s is at +%s words (agrees with second disassembly)\n[[jumptable]]\nsite = 0x%s\ncount = %s\n' "$tbl" "$tb" "$site" "$tb" >> funcs.toml
  else
    echo "$out"; exit 1
  fi
done
