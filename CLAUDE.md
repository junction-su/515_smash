# SMASH — Project Context

## Overview
WarioWare-style stress relief minigame collection controlled by a wearable knuckle device (button worn on finger knuckles). Single HTML file (`index.html`).

## Tech Stack
- Vanilla JS + CSS (no framework)
- Web Serial API (knuckle device connection, Chrome/Edge only)
- Single file: `index.html` (CSS + JS inline)

## Gesture → Game Action
| Device signal | Game action | Keyboard |
|---|---|---|
| `shake` | SMASH (fist clench) | `1` |
| `wave` | SWIPE (hand wave) | `2` |
| `idle` | nothing | `3` |

## Game Phases
`S.phase`: `TUTORIAL` → `WARMUP` → `MG_INTRO` → `MG_ACTIVE` → `MG_CLEAR` → `ROUND_CLEAR` → (repeat)

## Minigames
`MINIGAME_TYPES = ['wipe', 'crumple', 'swat', 'cloud']`

| Game | Gesture | Key function |
|---|---|---|
| wipe (window cleaning) | wave | `handleWipeWave()` |
| crumple (paper crumpling) | shake | `handleCrumpleShake()` |
| swat (fly swatting) | shake | `handleSwatShake()` |
| cloud (storm cloud blowing) | wave | `showFanSweep()` |

## Asset Structure
```
asset/
  sfx/          ← sound effects (mp3, 48kbps/22kHz)
    bag_punch.mp3
    swat_fly.mp3   (loop, 3s trimmed)
    swat_hit.mp3
    cloud_wind.mp3
    wipe_squeak.mp3
    crumple_paper.mp3
    crumple_trash.mp3
  *.webp         ← images (converted from PNG, 1920px max)
  knuckle_open.webp / knuckle_fist.webp  ← tutorial coach images
```

## CSS Variables (`:root`)
```css
--ink:   #111110   /* main text / borders */
--paper: #FAFAF8   /* background */
--dim:   #888880   /* subtext */
```
Fonts: `Geist` (900 headline) + `Geist Mono` (sub/mono)

## SFX System
```js
getSFX(key)    // lazy load + preload='auto' + .load()
playSFX(key)   // currentTime=0 then play
stopSFX(key)   // pause + reset
warmupSFX()    // called at first gesture() — silent play-pause forces decode on all audio
```
**Note**: `warmupSFX()` is called at the top of `gesture()` (covers keyboard, touch buttons, and serial device). Each `initXxx()` function preloads via `getSFX()`.

## Tutorial (desktop only)
- Mobile `@media (hover:none)` → `display:none`
- `S.phase = 'TUTORIAL'` → `gesture()` routes to `tutorialHit()` at top
- STEP 0: connect screen — card centered with dark dim overlay, slides left on connect/skip
- STEP 1: shake → `animateGlove()` + fist confirm
- STEP 2: wave → sandbag `tut-sway` + `cloud_wind` SFX
- Done: "LET'S GO!" flash → `exitIntro()` → WARMUP

## Branch
`claude/build-smash-html-PzfLq`
