# Roadmap

Features planned after the game boots and runs correctly in 4:3. Each item
links its issue; most are in the
[Platform features](https://github.com/ThisIsAkill/ct-recomp/milestone/7)
milestone. MSU-1 comes first.

**Where it goes**
- **engine**: `recomp/`, `runtime/`, `frontend/` (the future snesrecomp).
  Game-agnostic; any SNES game could use it.
- **game**: `game/ct/` data and hooks.
- **both**: the mechanism in the engine, the Chrono Trigger knowledge in
  `game/ct/` as data. These have one issue per side.

**Rules for every feature**
- **Hardcore mode:** anything that changes gameplay (save states, rewind,
  cheats, patches, slow motion, speed controls, autosave, practice tools) is
  disabled in RetroAchievements hardcore mode. The RetroAchievements user
  agent is always ct-recomp's own.
- **Lightweight:** nothing raises the minimum requirements. Heavy features
  (shaders, HD packs, interpolation) are optional and off by default.
- **Easy to use:** works out of the box with sensible defaults; setup never
  requires editing files.

## Achievements
| Feature | Where | Issue |
|---|---|---|
| RetroAchievements via rcheevos, softcore at launch, ct-recomp's own user agent | engine | #39 |
| Achievement definitions checked against the memory map | game | #40 |
| Hardcore mode, built from the start: locks out save states, rewind, cheats, patches, slow motion, speed controls, autosave, practice tools | engine | #53 |
| Leaderboards, rich presence, challenge/progress indicators, offline unlock sync | engine | #54 |
| Apply for hardcore approval after 6 months public | engine | #55 |

## Saves
| Feature | Where | Issue |
|---|---|---|
| Save states | engine | #35 |
| Autosave on every scene change, in CT's save format, 3 rotating slots; skipped during cutscenes, battles and event scripts | both | #45, #46 |
| `.srm` import from Snes9x, bsnes and Mesen (ships with PC Release) | engine | #56 |
| Cloud saves | engine | #57 |

## Cheats, patches, mods
| Feature | Where | Issue |
|---|---|---|
| Cheats: Pro Action Replay codes, data-only Game Genie codes | engine | #41 |
| Built-in, tested cheat list | game | #42 |
| IPS/BPS/UPS patches auto-applied at load (data-only), warning if a patch touches code; code changes go through the mod API | engine | #58 |
| Mod API for replacing compiled functions | both | #43, #44 |
| Scripting hooks on named functions (`recomp/` + `runtime/`) | engine | #59 |
| Readable generated C using ChronoRET labels as function names (emitter: engine; labels already in `game/ct/funcs.toml`) | both | #60 |
| Memory viewer and event flag editor | both | #61, #62 |
| Optional fixes for the original game's bugs | game | #63 |
| Jets of Time randomizer support (recompiling a patched ROM at load: engine) | both | #64, #65 |

## Graphics
| Feature | Where | Issue |
|---|---|---|
| Widescreen toggle with per-scene 4:3 fallback: wide-frame PPU rendering (engine); tile loading, sprite culling, cutscene and battle fixes (game). Order: Mode 7 and field maps, then battles, then cutscenes | both | #66, #67 |
| HD Mode 7 | engine | #68 |
| HD sprite and tile packs | engine | #69 |
| High refresh rate interpolation (120/144 Hz): framework (engine), camera and object positions (game) | both | #70, #71 |
| Sprite flicker removal; more than 128 sprites for mods | engine | #72 |
| Shaders, scaling and other graphics enhancements | engine | #38 |

## Audio
| Feature | Where | Issue |
|---|---|---|
| MSU-1 soundtrack support (first) | both | #51, #52 |
| Best-in-class audio: hardware-exact DSP by default, plus the options below | engine | #49, #50 |
| Separate music and SFX volume: mixer (engine), voice mapping (game) | both | #49, #73 |
| Interpolation options: Gaussian, cubic, sinc | engine | #49 |

## Text and accessibility
| Feature | Where | Issue |
|---|---|---|
| HD text rendering: renderer (engine), text-routine hook (game) | both | #74, #75 |
| Text-to-speech for dialogue | both | #76, #77 |
| Unicode localization: renderer (engine), string tables (game) | both | #78, #79 |
| Colorblind filters and text scaling | engine | #48 |
| Other accessibility options | engine | #48 |

## Controls and platforms
| Feature | Where | Issue |
|---|---|---|
| Remapping and controller support | engine | #36 |
| Touch and mouse menu controls: pointer input (engine), menu hooks (game) | both | #80, #81 |
| Steam Deck, handheld, controller-first UI | engine | #82 |
| PC first, then PS4/PS5 (PS5 milestone) | engine | #83 |
| Lightweight enough to run on any computer or console | rule | above |

## Quality of life
| Feature | Where | Issue |
|---|---|---|
| Speed controls: fast-forward and slow motion (engine); battle speed and text speed (game) | both | #84, #85 |
| Rewind (off in hardcore) | engine | #35 |
| Pause and settings | engine | #37 |
| Playtime tracking | engine | #47 |
| Discord rich presence | both | #86, #87 |
| Auto-updater and crash logs | engine | #88 |
| Very easy to use | rule | above |

## Speedrunning
| Feature | Where | Issue |
|---|---|---|
| In-game timer and LiveSplit autosplitter: timer and LiveSplit interface (engine), timing rules and splits (game) | both | #89, #90 |
| Practice tools: warps and flag editing (uses #61) | game | #91 |
