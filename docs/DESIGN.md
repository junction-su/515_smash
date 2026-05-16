# SMASH — Design Specification

## Design Philosophy

Editorial meets physical. Real objects on a clean background — like a magazine spread where the product just sits there, no staging, no fake shadows. The violence of the interaction (smashing, exploding) contrasts with the calm, minimal environment around it.

Reference: Favourite Routine editorial (Image 1) — objects floating on white with strong typography alongside. Not a game UI. Not a dashboard. A stage.

---

## Color

```
Background:    #E8ECF0   cool blue-gray (not warm, not cream)
Surface:       #FFFFFF   object cards, UI panels
Border:        rgba(0,0,0,0.08)
Text primary:  #1a1a18
Text muted:    #8a8880
Accent red:    #E53935   shake / impact
Accent blue:   #1565C0   wave / ripple
```

No gradients. No glow. No neon. Single accent per gesture.

---

## Typography

```css
font-family: 'Geist', sans-serif;           /* UI, body */
font-family: 'Geist Mono', monospace;       /* labels, status, counters */
```

### Scale
- Gesture name (big): `font-size: clamp(4rem, 10vw, 8rem)`, `font-weight: 900`, `letter-spacing: -0.04em`, `line-height: 0.85`
- Section label: `font-size: 10px`, `letter-spacing: 0.25em`, `text-transform: uppercase`, Geist Mono
- Body / status: `font-size: 12px`, `letter-spacing: 0.05em`, Geist Mono
- Counter: `font-size: 11px`, Geist Mono, muted

Rules:
- No Inter, no Roboto
- No emoji in UI
- Sentence case for labels, uppercase only for mono tags
- Body text never pure black — use `#1a1a18`

---

## Layout

### Grid
```
[Left panel 260px] | [Stage — remaining width]
```

Left panel:
- Wordmark top
- Big gesture label + sub-label center
- Controls + status bottom
- `border-right: 1px solid rgba(0,0,0,0.08)`
- `padding: 2.5rem 2rem`

Stage:
- Background `#E8ECF0`
- Sandbag hangs from top-center with chain
- Active stress object floats near sandbag
- Canvas layer for particles/effects behind everything

### Sandbag
- PNG image: white canvas bag, metal chain, photorealistic
- Hangs from top of screen
- Physics: pendulum swing via CSS transform + JS angle/velocity
- Reacts to gestures: big swing on shake, gentle sway on wave
- `transform-origin: top center`

### Stress Objects (Game Mode)
- One object at a time
- Floats in from edge with gentle bob animation
- PNG with transparent background, no drop shadow
- Sits to the side of the sandbag at varying heights
- After destroy → brief pause (600ms) → next object slides in

### Object Queue
```
[alarm clock] → [email icon] → [slack notification] → [invoice] → [repeat]
```
Randomize order. After full cycle → brief "cleared" moment → restart.

---

## Animation

### Idle State
- Sandbag: slow pendulum, ±3deg, 3s period
- Object: gentle float, `translateY` ±8px, 4s sine, slight rotation ±2deg

### Shake Effect
- Sandbag: immediate large swing (±25deg), damped oscillation back
- Object: flies off screen in random direction, rotates 360–720deg, scales to 0
- Particles: 80–120 rect/circle fragments from object position, physics (gravity, decay)
- Impact word: "SMASH!" / "POW!" / "CRUSH!" — appears center-stage, large, fades in 80ms, fades out over 400ms
- Screen flash: `rgba(229,57,53,0.06)` overlay for 100ms
- Screen shake: `translate` ±6px for 8 frames

### Wave Effect
- Sandbag: medium swing (±12deg)
- Object: splits into two halves, slide apart, fade out
- Ripples: 4–5 expanding circles from object center, blue palette
- Particles: 30–40 smaller fragments, arc trajectory

### Idle (no gesture)
- Nothing happens visually
- Label updates to "IDLE"

---

## Components

### Left Panel
```
┌─────────────────────────┐
│ SMASH · Gesture UI      │  ← wordmark, 11px mono muted
│                         │
│                         │
│ SHAKE                   │  ← big label, 900 weight
│ impact detected · #12   │  ← sub label, mono muted
│                         │
│                         │
│ ● connected             │  ← status dot + text
│ [ r ]  record gesture   │  ← primary button
│ connect device          │  ← outline button
└─────────────────────────┘
```

### Buttons
```css
/* Primary */
background: #1a1a18;
color: #fff;
border: none;
padding: 0.7rem 1rem;
font-family: Geist Mono;
font-size: 10px;
letter-spacing: 0.15em;
text-transform: uppercase;

/* Outline */
background: transparent;
color: #1a1a18;
border: 1px solid rgba(0,0,0,0.12);

/* Hover */
transform: scale(0.97) on :active
```

No border-radius (or max 2px). No box-shadow.

### Status Dot
```css
width: 6px; height: 6px; border-radius: 50%;
background: #ccc;                    /* disconnected */
background: #52c41a;                 /* connected */
```

---

## Assets Required

| File | Description | Source |
|------|-------------|--------|
| `sandbag.png` | White canvas sandbag, chain, transparent bg | Image 2, remove.bg |
| `alarm.png` | Flip clock / alarm clock, transparent bg | Image 3, remove.bg |
| `email.png` | Email app icon with badge, transparent bg | Image 4, remove.bg |
| `slack.png` | Slack notification, transparent bg | Custom SVG or screenshot |
| `invoice.png` | Bill/invoice, transparent bg | Custom SVG |

Recommended: process through [remove.bg](https://remove.bg) for clean transparent PNGs.

Fallback: SVG illustrations if PNGs not available (see `smash.html` SVG definitions).

---

## Canvas Particle System

```js
class Particle {
  // rect or circle shape
  // gravity: 0.5
  // decay: 0.016–0.028
  // rotation with rotV
  // vx *= 0.97 per frame (air resistance)
}

class Ripple {
  // expanding circle
  // speed: 9px/frame
  // alpha decay: 0.01/frame
}
```

Animate exclusively via `transform` and `opacity`. No `top`/`left` animation.

---

## Interaction States

| State | Big Label | Sub Label | Dot |
|-------|-----------|-----------|-----|
| Disconnected | — | awaiting connection | gray |
| Connected, idle | — | awaiting gesture | green |
| Recording | RECORDING | … | green, pulse |
| Shake detected | SHAKE | impact detected · #N | green |
| Wave detected | WAVE | slice detected · #N | green |
| Idle detected | IDLE | no gesture · #N | green |

---

## Responsive

Desktop only (1280px+ recommended for demo). No mobile support needed.
Min viable width: 900px.
