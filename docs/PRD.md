# SMASH — Product Requirements Document

## Overview

SMASH is a stress-relief gesture interface. A wrist-worn device (XIAO ESP32S3 + MPU6050) classifies physical gestures in real time using an on-device TFLite model, then sends the result over USB Serial to a browser-based visual experience. The browser renders satisfying destruction animations — punching a sandbag, smashing stress objects — triggered by the user's physical motion.

This is the capstone individual project for TECHIN 515 (Hardware Software Lab II, Spring 2026).

---

## Problem

GIX students cycle through back-to-back deadlines, code reviews, and presentations with no physical outlet for accumulated stress. Existing stress apps are passive (breathing timers, ambient sounds). SMASH creates an active, embodied feedback loop: a physical gesture produces an immediate, satisfying digital consequence.

---

## Goals

- End-to-end gesture recognition pipeline: data collection → model training → on-device inference → browser visual
- Demonstrate resource-constrained optimization (model size vs. accuracy trade-off)
- Deliver a demo-ready artifact: plug in the device, open the browser, smash things

---

## Non-Goals

- Mobile app
- BLE/wireless (USB Serial is sufficient for demo)
- Multi-user
- Cloud inference

---

## System Architecture

```
[Wrist device]         [USB Serial]      [Browser]
XIAO ESP32S3        ──────────────►   smash.html
+ MPU6050                             Web Serial API
TFLite model                          Canvas animations
(Random Forest)
```

### Hardware
- XIAO ESP32S3
- MPU6050 (I2C, SDA=D4, SCL=D5)
- USB-C cable (for serial + power during demo; LiPo for final)

### Firmware (Arduino)
- Reads 100 IMU samples at 50Hz (2s window)
- Extracts 30 features (mean, std, max, min, abs_max per 6 axes)
- Runs Random Forest inference via `gesture_model.h` (micromlgen)
- Sends gesture label over Serial at 115200 baud when triggered by `r` command

### ML Model
- Algorithm: Random Forest, n_estimators=10
- Classes: `shake`, `wave`, `idle`
- Dataset: 950 samples (310 shake, 320 wave, 320 idle)
- Features: 30 (mean/std/max/min/abs_max × ax/ay/az/gx/gy/gz)
- Test accuracy: 95.8%
- Model size: 58KB (converted via micromlgen)
- Trade-off: 100 trees → 562KB @ 96.5% vs 10 trees → 58KB @ 95.8%

### Browser (smash.html)
- Connects to ESP32S3 via Web Serial API (Chrome only)
- Receives gesture label line over serial
- Triggers visual effect per gesture
- Keyboard shortcuts for demo: `1`=shake, `2`=wave, `3`=idle

---

## Gesture → Visual Mapping

| Gesture | Physical action | Visual effect |
|---------|----------------|---------------|
| `shake` | Hard shake / punch motion | Sandbag swings hard, stress objects explode outward, screen shake, particle burst |
| `wave` | Slow wave / sweep | Sandbag sways gently, objects drift/fade, ripple effect |
| `idle` | Still / resting | Sandbag swings to rest, objects float peacefully |

---

## Visual Design

### Aesthetic Reference
- Image 1 (Favourite Routine editorial): white/light background, real objects floating with no drop shadows, strong editorial typography alongside
- Image 5 (stacked cards): objects presented as a stack, one revealed at a time
- Sandbag: white canvas sandbag with metal chain, photorealistic PNG (remove.bg)
- Stress objects: alarm clock, email icon, slack notification, invoice/bill — photorealistic PNGs

### Layout
- Background: `#E8ECF0` (cool blue-gray, not warm cream)
- Sandbag centered/slightly right, hangs from top with chain, swings with physics
- Stress objects appear one at a time floating around the sandbag (game mode)
- After shake destroys an object → next object spawns
- Strong editorial typography on left: gesture name, count, status
- Font: Geist (sans) + Geist Mono (mono labels)
- No gradients, no heavy shadows, no glow effects
- Objects are PNG with transparent background (no drop shadows on them)

### Game Loop
1. Object floats in from edge, bobs gently near sandbag
2. User performs gesture
3. Object explodes/slices/flies off screen with satisfying animation
4. Brief pause → next object spawns
5. After all objects cleared → "YOU WIN" moment, restart

---

## File Structure

```
smash/
├── smash.ino              # Arduino firmware
├── gesture_model.h        # ML model (micromlgen output)
├── smash.html             # Browser visual (single file, no build step)
├── assets/
│   ├── sandbag.png        # Sandbag PNG with transparent bg
│   ├── alarm.png          # Alarm clock PNG
│   ├── email.png          # Email icon PNG
│   ├── slack.png          # Slack notification PNG
│   └── invoice.png        # Invoice/bill PNG
├── data_collection/
│   ├── collect_data.py
│   └── gesture_data/      # 950 CSV samples
├── model/
│   ├── train_model.py
│   ├── convert_model.py
│   └── gesture_model.pkl
└── docs/
    ├── PRD.md
    ├── DESIGN.md
    └── CLAUDE.md
```

---

## Success Criteria

| Metric | Target | Achieved |
|--------|--------|----------|
| Gesture accuracy | ≥85% | 95.8% |
| End-to-end latency | <150ms | ~120ms |
| Model flash size | <1MB | 58KB |
| Battery life | ≥4h (500mAh) | TBD |
| Classes | 3 | 3 (shake/wave/idle) |

---

## Risks & Mitigations

| Risk | Mitigation |
|------|-----------|
| Web Serial not supported | Chrome only; document requirement |
| PNG assets not ready | Fallback to SVG illustrations |
| Serial port conflict (Arduino IDE open) | Close Arduino IDE before opening browser |
| Model accuracy drops on new user | Collected from 1 user; document limitation |

---

## Demo Script

1. Open `smash.html` in Chrome
2. Click "connect device" → select `cu.usbmodem...`
3. Press `r` (or button) → perform shake gesture → object explodes
4. Repeat for all objects
5. Fallback: keyboard `1`/`2`/`3` for demo without device

---

## Timeline (Weeks 1–10)

| Week | Milestone |
|------|-----------|
| 1 | Hardware wiring + IMU data confirmed |
| 2 | Data collection pipeline + 950 samples |
| 3 | Baseline model trained (95.8% accuracy) |
| 4 | Model quantization + trade-off documented |
| 5 | TFLite deployed to ESP32S3, inference running |
| 6 | Browser visual v1 (SVG placeholders) |
| 7 | PNG assets + game loop complete |
| 8 | Battery + enclosure |
| 9 | User testing + final benchmarks |
| 10 | Demo day |
