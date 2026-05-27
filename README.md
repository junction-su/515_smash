# SMASH

A wearable gesture-controlled stress relief game. Clench your fist and wave your hand to smash through minigames using a knuckle-worn device.

TECHIN 515 — Hardware Software Lab II, Spring 2026 capstone project.

---

## Running the Demo

**Browser: Chrome or Edge required** (Web Serial API)

1. Open `index.html` in Chrome
2. If you have the device, click "connect device" → select the USB port
3. Keyboard fallback works without a device

On mobile, the app shows a short touch demo start screen instead of the device onboarding flow. Desktop with the knuckle device is recommended, but the game can be tried with the on-screen **SMASH** / **SWIPE** controls.

| Key | Gesture |
|-----|---------|
| `1` | SMASH (fist clench) |
| `2` | SWIPE (hand wave) |

---

## Minigames

| Game | Gesture | Description |
|------|---------|-------------|
| **Wipe** | Swipe | Clean a dirty window |
| **Crumple** | Smash | Crumple paper and toss it in the bin |
| **Swat** | Smash | Swat flies buzzing around |
| **Cloud** | Swipe | Blow away storm clouds |

---

## System Architecture

```
[Knuckle device]        [USB Serial]       [Browser]
XIAO ESP32S3        ──────────────►    index.html
+ MPU6050                              Web Serial API
TFLite model                           Canvas animations
```

### Hardware
- XIAO ESP32S3
- MPU6050 (I2C: SDA=D4, SCL=D5)
- USB-C cable

### Firmware (`smash/smash.ino`)
- Collects 100 IMU samples at 50Hz (2s window)
- Extracts 30 features (mean/std/max/min/abs_max × 6 axes)
- Runs Random Forest inference → sends gesture label over Serial (115200 baud)

### ML Model

| | |
|--|--|
| Algorithm | Random Forest (n=10) |
| Classes | `shake` / `wave` / `idle` |
| Training data | 950 samples |
| Test accuracy | 95.8% |
| Model size | 58KB |
| End-to-end latency | ~120ms |

---

## File Structure

```
515_smash/
├── index.html              # Browser game (inline CSS + JS, no build step)
├── asset/
│   ├── sfx/               # Sound effects (mp3, 48kbps/22kHz)
│   └── *.webp             # Game images (WebP optimized)
├── smash/
│   ├── smash.ino          # ESP32S3 Arduino firmware
│   └── gesture_model.h    # Trained model (micromlgen output)
├── gesture/
│   └── gesture.ino        # Data collection sketch
├── collect_data.py         # Data collection script
├── train_model.py          # Model training
├── convert_model.py        # Convert model to C header
└── docs/
    ├── PRD.md
    └── DESIGN.md
```

---

## Retraining the Model

```bash
# 1. Collect data (after uploading gesture.ino)
python collect_data.py

# 2. Train model
python train_model.py

# 3. Convert to firmware header
python convert_model.py
# → updates smash/gesture_model.h, then re-upload smash.ino
```

---

## Notes

- Web Serial requires **Chrome or Edge** (not supported in Firefox)
- Close Arduino IDE before connecting in the browser to avoid Serial port conflicts
- Full game is playable with keyboard `1` / `2` — no device needed
- Mobile/touch browsers skip device onboarding and use on-screen SMASH / SWIPE buttons
