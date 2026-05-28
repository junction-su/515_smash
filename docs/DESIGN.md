# SMASH — Design Specification

## Design Philosophy

SMASH uses a full-screen stage. Minigame objects fill the viewport, while the HUD sits as a small floating control at the bottom. There is no left-side panel; typography is treated editorially and placed in the upper-right corner.

---

## Color

```css
--ink:   #111110   /* primary text */
--dim:   #8A8880   /* muted labels */
--red:   #D4351C   /* smash accent */
--go:    #16A34A   /* connected state */
--paper: #FAFAF8   /* paper background */

Stage background: #FFFFFF
HUD background:   rgba(226,231,238,0.65) + backdrop-filter:blur(14px)
Ghost text:       rgba(17,17,16,0.032)
```

Avoid adding new box shadows, border radii, or gradients unless the change explicitly calls for them.

---

## Typography

```css
'Geist', sans-serif          /* UI and body */
'Geist Mono', monospace      /* labels, status, counters */
'Playfair Display', italic   /* editorial script headings */
```

- Minigame prompt: `clamp(5rem,13vw,10rem)`, weight 900
- Gesture hint label: `Geist Mono 11px`, `letter-spacing:0.28em`
- HUD label: `Geist Mono 10px`, `letter-spacing:0.1em`
- Counter: `Geist Mono 13px`, `letter-spacing:0.22em`

---

## Layout

```
[Full-screen Stage #FFFFFF]
  ├── #canvas (z:0)            particles
  ├── #ghost (z:0)             background word
  ├── minigame layers (z:3~10)
  ├── #typo-block (z:25)       upper-right editorial type
  ├── #cleared (z:25)          round/minigame clear overlay
  ├── #mg-intro (z:100)        prompt flash
  ├── #mg-gesture-hint (z:60)  animated gesture SVG hint
  ├── #hud-container (z:30)    desktop bottom floating pill
  ├── #touch-btns (z:30)       mobile SMASH/SWIPE controls
  ├── #intro-overlay (z:80)    desktop onboarding
  └── #mobile-intro (z:90)     mobile touch demo start screen
```

Reference viewport: `width=1280`.

---

## Sandbag

```
#bag-wrap: top:0; left:50%; transform-origin:top center
#bag-cord: 1px wide, 60px tall gradient cord
#bag-img: height:clamp(420px,68dvh,820px)
```

---

## Minigame Intro Flash + Gesture Hint

```
#mg-intro: top:42%, large prompt text (scale 0.75 → 1 pop-in)
#mg-gesture-hint: top:58%, SVG icon + label below prompt
```

- SMASH games: `fist.svg` + `@keyframes smash-hint` vertical bounce, 0.38s
- SWIPE games: `palm.svg` + `@keyframes swipe-hint` horizontal sweep, 0.85s
- Both disappear after 1.1s

---

## Minigame Visuals

### WIPE IT — window cleaning (swipe)

```
#mg-wipe-bg: inset:0; z-index:6
#wipe-img: inset:5%; 90% x 90%; object-fit:cover
  -> window_00~04.png, advancing one frame per swipe
#rag-el: 420px; image swaps at sweep midpoint (50%)
Clear state: "Spotless/CLEARED" overlay on window_04 for 2.2s
```

### CRUMPLE! — paper crumpling (smash)

```
crumple_00~04.png: centered, advances one stage per smash
hand_open.png: idle hand state
hand_close.png: momentary smash feedback
After crumple_04: trash.png appears at lower-right
-> trash_open.png swap -> crumpled paper flies into the bin
```

### SWAT IT! — fly swatter (smash)

```
.fly: 72px, five flies placed by zones, transform-only movement
#swatter-el: 700px; top-down slam animation
Hit logic: 70% targeting chance + +/-140px error
```

### BLOW IT! — cloud clearing (swipe)

```
10 clouds, three row sectors:
  row0: bottom-edge anchor, partially off the top of the screen
  row1: top 16~38%
  row2: top 60~76%
#fan-el: bottom:22%; fan sweep animation at +/-55deg
bottom white gradient: rgba(255,255,255,0.5), z-index:9
reveal: sky 0ms -> field 500ms -> sun 900ms
-> text overlay at 2000ms -> finish at 3600ms
```

---

## WarioWare Prompt Flash

```css
#mg-intro: position absolute; top:42%; z-index:100
transition: opacity 0.1s, transform 0.15s cubic-bezier(0.16,1,0.3,1)
.show: opacity:1; scale(1)
```

---

## HUD

```
position:absolute; bottom:1.75rem; left:50%
background: rgba(226,231,238,0.65); backdrop-filter:blur(14px)
dot + status text + separator + connect button
keyboard hint: "keyboard: 1 smash · 2 swipe"
```

| State | Dot |
|---|---|
| Disconnected | rgba(30,42,60,0.3) |
| Connected | #16A34A |
| Recording | #16A34A blink |

---

## Mobile Touch Demo

Mobile/touch environments skip desktop device onboarding and show `#mobile-intro` first.

```
#mobile-intro:
  background rgba(250,250,248,0.72)
  backdrop-filter blur(12px)
  centered full-screen overlay

#mobile-intro-visual:
  asset/knuckle_device.png
  overflow:visible
  subtle float animation

#touch-btns:
  bottom: calc(1rem + env(safe-area-inset-bottom))
  two equal buttons: SMASH / SWIPE
  data-needed="smash|swipe" dims the inactive action
```

- START DEMO enters `WARMUP` after a 0.28s fade/translate transition.
- Mobile onboarding should feel like a start screen over a blurred game stage, not a boxed pop-up card.
- The favicon uses `asset/favicon.png`, a 512px square canvas version of `knuckle_device.png` with preserved aspect ratio.
