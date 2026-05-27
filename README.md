# SMASH

스트레스 해소용 웨어러블 제스처 게임. 손목 너클 디바이스로 실제 주먹을 쥐고 손을 흔들어 미니게임을 클리어한다.

TECHIN 515 (Hardware Software Lab II, Spring 2026) 캡스톤 프로젝트.

---

## 데모 실행 방법

**브라우저: Chrome / Edge 필수** (Web Serial API 지원)

1. `index.html`을 Chrome에서 열기
2. 디바이스가 있으면 "connect device" → USB 포트 선택
3. 디바이스 없이 키보드로도 플레이 가능

| 키 | 제스처 |
|----|--------|
| `1` | SMASH (주먹 쥐기) |
| `2` | SWIPE (손 흔들기) |

---

## 미니게임

| 게임 | 제스처 | 설명 |
|------|--------|------|
| **Wipe** | Swipe | 더러운 창문 닦기 |
| **Crumple** | Smash | 종이 구겨서 쓰레기통에 던지기 |
| **Swat** | Smash | 날아다니는 파리 잡기 |
| **Cloud** | Swipe | 먹구름 날려버리기 |

---

## 시스템 구성

```
[너클 디바이스]          [USB Serial]       [브라우저]
XIAO ESP32S3        ──────────────►    index.html
+ MPU6050                              Web Serial API
TFLite 모델                             Canvas 애니메이션
```

### 하드웨어
- XIAO ESP32S3
- MPU6050 (I2C: SDA=D4, SCL=D5)
- USB-C 케이블

### 펌웨어 (`smash/smash.ino`)
- MPU6050에서 100샘플 수집 (50Hz, 2초 윈도우)
- 30개 특징 추출 (mean/std/max/min/abs_max × 6축)
- Random Forest 추론 → Serial로 제스처 레이블 전송 (115200 baud)

### ML 모델

| 항목 | 값 |
|------|----|
| 알고리즘 | Random Forest (n=10) |
| 클래스 | `shake` / `wave` / `idle` |
| 학습 데이터 | 950샘플 |
| 테스트 정확도 | 95.8% |
| 모델 크기 | 58KB |
| 추론 레이턴시 | ~120ms |

---

## 파일 구조

```
515_smash/
├── index.html              # 브라우저 게임 (CSS + JS 인라인, 빌드 불필요)
├── asset/
│   ├── sfx/               # 효과음 (mp3, 48kbps/22kHz)
│   └── *.webp             # 게임 이미지 (WebP 최적화)
├── smash/
│   ├── smash.ino          # ESP32S3 Arduino 펌웨어
│   └── gesture_model.h    # 학습된 모델 (micromlgen 변환)
├── gesture/
│   └── gesture.ino        # 제스처 데이터 수집용 스케치
├── collect_data.py         # 데이터 수집 스크립트
├── train_model.py          # 모델 학습
├── convert_model.py        # TFLite → C 헤더 변환
└── docs/
    ├── PRD.md
    └── DESIGN.md
```

---

## 모델 재학습

```bash
# 1. 데이터 수집 (gesture.ino 업로드 후)
python collect_data.py

# 2. 모델 학습
python train_model.py

# 3. 펌웨어용 헤더 변환
python convert_model.py
# → smash/gesture_model.h 갱신 후 smash.ino 재업로드
```

---

## 주의사항

- Web Serial은 **Chrome / Edge** 전용 (Firefox 미지원)
- 펌웨어 업로드 시 Arduino IDE를 닫아야 Serial 포트 충돌 방지
- 디바이스 없이도 키보드 `1` / `2`로 전체 게임 플레이 가능
