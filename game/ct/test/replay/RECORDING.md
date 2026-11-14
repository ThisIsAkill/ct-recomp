# Recording replay chapters

100% native coverage needs replays of the whole game. They are recorded
in Mesen 2 as ~30-minute chapters: each chapter is one `.mmo` movie that
starts from power-on with the in-game save the previous chapter ended
with, so the chain can be replayed headlessly from the beginning
([#109](https://github.com/ThisIsAkill/ct-recomp/issues/109)).

## Before chapter 1

- Mesen 2 (2.x; movies from Mesen 1.x are refused).
- The US ROM this repo is built against: verify yours with
  `tools/check_rom.py game/ct <ROM>` (size and CRC32 from `game.toml`).
  The converter also checks the movie's ROM SHA-1 against the ROM.
- Settings → SNES: **RAM power-on state = All zeros** (no random
  power-on state). This is the state the probe powers on with; a movie
  recorded with anything else is refused.
- Port 1: SNES controller. Port 2: nothing (any pad-2 input is refused).
- No cheats enabled, no ROM patch applied (both are refused).
- Everything else can stay at defaults. Input is recorded per poll, so
  display and speed settings do not matter; play at a speed you like.

## Each chapter

1. **Chapter 1:** Tools → Movies → Record, **"Start without save data"**,
   and start a new game.
   **Later chapters:** with the same ROM loaded and its `.srm` holding
   the previous chapter's final in-game save (step 3), record with
   **"Start with save data"**, then load that save from the title screen.
   "Record from current state" embeds a savestate and is refused.
2. Play ~30 minutes. **Never press reset or power** — end the chapter
   instead (a reset mid-chapter is refused; battery SRAM survives resets
   either way, so nothing is gained).
3. **Save in-game** (save point / world map) and only then stop the
   recording. That save, as Mesen's `.srm` for the ROM, is what the next
   chapter starts with; until you save in-game, the battery does not
   hold your progress.
4. Name the files in order: `01-<slug>.mmo`, `02-<slug>.mmo`, … in this
   directory.

## Convert and verify the chain

```
python tools/chapters.py --probe build/ct_boot --rom "$CT_ROM" \
    game/ct/test/replay/01-*.mmo game/ct/test/replay/02-*.mmo ...
```

This converts each chapter to an input script (`<stem>.txt`) plus the
save it starts from (`<stem>.start.srm`), replays it headless, and fails
unless each chapter starts from exactly the save the replay of the
previous one ended with (`<stem>.end.srm`). If a chapter is refused,
`tools/tas_convert.py` says which rule it broke (savestate, cheats,
reset/power press, pad 2, non-reference settings, …).

Commit the `.mmo`, `.txt`, and `.start.srm` files, and add a lockstep
test per chapter in `game/ct/CMakeLists.txt` (the movies and saves are
recordings and battery contents, not game assets):

```
add_test(NAME lockstep_chapter_01 COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames <N>
    --script ${GAME_DIR}/test/replay/01-<slug>.txt
    --sram-in ${GAME_DIR}/test/replay/01-<slug>.start.srm ...)
```
