# CLAUDE.md — SMASH

Single-file browser app. No build step. Vanilla HTML/CSS/JS + Canvas.

## Files

```
index.html          <- full game, single HTML/CSS/JS file
asset/              <- all visual and audio assets
docs/DESIGN.md      <- visual design specification
smash/smash.ino     <- Arduino firmware; avoid editing unless firmware changes are requested
smash/gesture_model.h <- ML model header; avoid editing unless retraining/conversion is requested
```

## Stack

- Vanilla HTML/CSS/JS only. No framework, no bundler.
- Canvas API for particles.
- Web Serial API for device connection, Chrome/Edge only.
- Google Fonts: Geist + Geist Mono + Playfair Display.

## Game Structure

WarioWare-style minigame loop. Each round selects four random minigames from the active set.

```
Sandbag warmup (4-5 smash actions) -> 4 random minigames -> ROUND CLEAR -> repeat
```

Phases:

```
TUTORIAL (desktop only)
MOBILE_INTRO (touch only)
WARMUP
MG_INTRO
MG_ACTIVE
MG_CLEAR
ROUND_CLEAR
```

## Minigames

| ID | Prompt | Gesture | Status |
|---|---|---|---|
| `bubble` | POP IT! | smash | disabled/commented out |
| `wipe` | WIPE IT! | swipe | `window_00~04` + `rag.png` |
| `crumple` | CRUMPLE! | smash | `crumple_00~04` + `trash.png/trash_open.png` + `hand_open/close.png` |
| `swat` | SWAT IT! | smash | five `fly.webp` elements + `swatter.webp` |
| `cloud` | BLOW IT! | swipe | ten `cloud.png` elements + `fan.png` + sky/field/sun reveal |

Enable/disable minigames through the `MINIGAME_TYPES` array.

## Assets

```
asset/sandbag.webp
asset/glove_left.webp, glove_right.webp
asset/fist.svg, palm.svg              <- intro gesture hint icons
asset/knuckle_open.webp, knuckle_fist.webp
asset/knuckle_device.png              <- device-only mobile intro/deck image
asset/favicon.png                     <- square favicon generated from knuckle_device.png
asset/bubble_00~06.png                <- currently disabled
asset/window_00~04.webp, rag.png
asset/crumple_00~04.png
asset/trash.png, trash_open.png
asset/hand_open.png, hand_close.png
asset/swatter.webp, fly.webp
asset/cloud.png, fan.png, sky.webp, field.png, sun.png
asset/sfx/*.mp3
```

## Key JS Structure

```js
const S = { phase, warmupHits, warmupTarget, mgQueue, currentMG, ... }
const MG = { bubble, wipe, crumple, swat, cloud }

function gesture(raw)         // 'shake' | 'wave' | 'idle', matching the device protocol
function initMinigame(type)   // initialize minigame UI/state
function mgClear(type)        // clear minigame -> next
function hideMinigameUI(type) // immediate display:none cleanup
function startNextMinigame()  // MG_INTRO flash + gesture hint -> initMinigame
```

## Gesture Naming

- Device protocol: `'shake'` / `'wave'` / `'idle'`
- UI labels: **SMASH** for shake, **SWIPE** for wave
- Intro hint: SMASH uses `fist.svg` vertical bounce; SWIPE uses `palm.svg` horizontal sweep
- `#mg-gesture-hint` is shown with the intro text for 1.1s

## Input

```
1 -> smash (shake)
2 -> swipe (wave)
r -> record, only when a device is connected
```

Mobile/touch controls:

```
#touch-smash -> gesture('shake')
#touch-swipe -> gesture('wave')
#touch-btns[data-needed="smash"|"swipe"] dims the inactive action
```

## Web Serial

```js
port = await navigator.serial.requestPort();
await port.open({ baudRate: 115200 });
// receive: 'shake' | 'wave' | 'idle'
// ignore: 'READY', 'START', 'END', 'Recording...'
// send: 'r'
```

## Onboarding

Desktop:

- `(hover:hover) and (pointer:fine)` starts the desktop tutorial.
- Step 0 connects or skips the device.
- Step 1 teaches SMASH: "punch forward with the device — or press 1".
- Step 2 teaches SWIPE: "swipe sideways with the device — or press 2".
- Completion shows "LET'S GO!" then enters `WARMUP`.

Mobile:

- Touch/coarse-pointer environments skip desktop onboarding.
- `#mobile-intro` appears with `knuckle_device.png`, a START DEMO button, and blurred stage background.
- START DEMO fades out over 0.28s and enters `WARMUP`.
- DevTools desktop-to-mobile switching is handled by the pointer media query change listener.

## Rules

- Animate only `transform` and `opacity` unless there is a strong reason.
- Use `hideMinigameUI()` for immediate minigame cleanup.
- Keep z-index relationships aligned with `docs/DESIGN.md`.
- Avoid adding new shadows, rounded containers, or gradients unless the change explicitly needs them.
- Use `spawnParts(x, y, count)` for particle bursts.

## Clear Flow

- `wipe`: `showWipeClear()` -> "Spotless/CLEARED" overlay on `window_04` for 2.2s -> next
- `cloud`: `showCloudReveal()` -> sky 0ms -> field 500ms -> sun 900ms -> text at 2000ms -> finish at 3600ms
- Other minigames: `mgClear(type)` -> `animImpact` -> `startNextMinigame` after 900ms
