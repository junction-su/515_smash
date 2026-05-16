# CLAUDE.md — SMASH Project

This file tells Claude Code how to work on this project.

---

## Project

SMASH is a single-page browser app (`smash.html`) that visualizes physical gesture data from a wrist-worn ESP32S3 device. No build step. No framework. Vanilla HTML/CSS/JS + Canvas.

---

## Key Files

```
smash.html          ← THE main deliverable (single file)
assets/             ← PNG images with transparent backgrounds
  sandbag.png
  alarm.png
  email.png
  slack.png
  invoice.png
smash.ino           ← Arduino firmware (do not modify unless asked)
gesture_model.h     ← ML model header (do not modify)
docs/PRD.md         ← Full product spec
docs/DESIGN.md      ← Visual design spec (READ THIS FIRST)
```

---

## Rules

### Always read DESIGN.md before touching smash.html
Design decisions are documented there. Do not deviate without explicit user instruction.

### Stack
- Vanilla HTML/CSS/JS only
- No React, no Vue, no build tools
- Canvas API for particles and effects
- Web Serial API for device connection
- Google Fonts: Geist + Geist Mono (already linked)

### Colors — use exactly these
```
Background:  #E8ECF0
Text:        #1a1a18
Muted:       #8a8880
Border:      rgba(0,0,0,0.08)
Accent red:  #E53935  (shake)
Accent blue: #1565C0  (wave)
```
No other colors without asking.

### Typography rules
- Geist for UI, Geist Mono for labels/status/counters
- No Inter, no Roboto, no system fonts
- No emoji anywhere
- Uppercase only for Geist Mono short labels (status tags, kbd shortcuts)
- Sentence case for everything else

### Layout rules
- Left panel: 260px fixed, `border-right: 1px solid rgba(0,0,0,0.08)`
- Stage: remaining width, background `#E8ECF0`
- Sandbag hangs from top of stage, center
- One stress object active at a time (game loop)
- Canvas sits behind everything (`z-index: 0`)

### Animation rules
- Animate ONLY `transform` and `opacity` — never `top`, `left`, `width`, `height`
- Sandbag: pendulum physics (angle + velocity + damping), `transform-origin: top center`
- Particles: gravity + air resistance, rect and circle shapes
- No CSS gradients on backgrounds
- No box-shadow (except ultra-subtle `0 1px 3px rgba(0,0,0,0.04)` on surface elements)
- No glow, no neon, no blur (except `backdrop-filter` on modal if needed)

### Asset handling
```js
// If PNG exists, use it
<img src="assets/sandbag.png" alt="" />

// If PNG missing, fall back to inline SVG
// SVG fallbacks are defined in smash.html
```

Always check if asset file exists before referencing. If `assets/` folder is empty, use SVG fallbacks.

### Web Serial
```js
// Connect
port = await navigator.serial.requestPort();
await port.open({ baudRate: 115200 });

// Write (trigger record)
await writer.write('r');

// Read loop — parse gesture labels
// Valid labels: 'shake', 'wave', 'idle'
// Ignore: 'READY', 'START', 'END', 'Recording...'
```

### Game Loop
```
State machine:
IDLE → WAITING → OBJECT_ACTIVE → DESTROYING → PAUSE → OBJECT_ACTIVE → ...

IDLE:          No gesture received yet. Sandbag sways. No object shown.
WAITING:       Device connected. First object spawning.
OBJECT_ACTIVE: Object floating near sandbag. Waiting for gesture.
DESTROYING:    Gesture received. Destruction animation playing.
PAUSE:         600ms gap. Object gone. Next object loading.
```

Object queue: `['alarm', 'email', 'slack', 'invoice']` — randomize each cycle.

### Keyboard shortcuts (demo mode — no device needed)
```
1 → shake
2 → wave
3 → idle
r → trigger record (if device connected)
```

---

## What NOT to do

- Do not add a framework or bundler
- Do not use localStorage or sessionStorage
- Do not add more than 2 font families
- Do not add border-radius > 4px on buttons
- Do not use `h-screen` — use `min-height: 100dvh` if needed
- Do not add purple/violet/gradient to anything
- Do not center the main layout (left-aligned left panel is intentional)
- Do not add tooltip libraries or icon libraries (use inline SVG for icons)
- Do not make the UI responsive for mobile (desktop only)

---

## Testing Without Device

Open `smash.html` in Chrome. Press `1`, `2`, `3` on keyboard to simulate gestures.
Do NOT need to connect device to test visuals.

---

## Common Tasks

### Add a new stress object
1. Add PNG to `assets/` folder
2. Add SVG fallback to `FALLBACK_SVGS` object in smash.html
3. Add key to `OBJECT_QUEUE` array

### Tune particle physics
- `gravity`: higher = faster fall (default 0.5)
- `decay`: higher = faster fade (default 0.018)
- Count: shake=100, wave=50

### Tune sandbag swing
- `punchForce`: shake=14, wave=6
- `damping`: 0.93 (lower = more oscillation)

### Change gesture colors
Edit CSS variables at top of `<style>` block only.

---

## Device Connection Troubleshooting

- Close Arduino IDE Serial Monitor before opening browser (port conflict)
- Use Chrome only (Firefox/Safari don't support Web Serial)
- If port not showing: unplug and replug USB, refresh page
- Baud rate must match firmware: 115200
