# CLAUDE.md — SMASH

Single-file browser app. No build step. Vanilla HTML/CSS/JS + Canvas.

## Files
```
smash.html          ← 전체 게임 (HTML+CSS+JS 단일 파일)
asset/              ← 모든 에셋 (PNG + SVG, transparent bg)
docs/DESIGN.md      ← 비주얼 디자인 스펙
smash.ino           ← Arduino 펌웨어 (수정 금지)
gesture_model.h     ← ML 모델 헤더 (수정 금지)
```

## Stack
- Vanilla HTML/CSS/JS only. No framework, no bundler.
- Canvas API: 파티클 시스템
- Web Serial API: 디바이스 연결 (Chrome only)
- Google Fonts: Geist + Geist Mono + Playfair Display

## Game Structure

**WarioWare 스타일.** 5개 미니게임 중 매 라운드 랜덤 4개 선택.
```
샌드백 워밍업 (4~5 smash) → 미니게임 4개 랜덤 → ROUND CLEAR → 반복
```

게임 페이즈: `WARMUP` → `MG_INTRO` → `MG_ACTIVE` → `MG_CLEAR` → `ROUND_CLEAR`

## 미니게임 목록

| ID | 지시문 | 제스처 | 상태 |
|---|---|---|---|
| `bubble` | POP IT! | smash | **비활성** (주석처리) |
| `wipe` | WIPE IT! | swipe | `window_00~04.png` + `rag.png` |
| `crumple` | CRUMPLE! | smash | `crumple_00~04.png` + `trash.png/trash_open.png` + `hand_open/close.png` |
| `swat` | SWAT IT! | smash | `fly.png` 5마리 + `swatter.png` |
| `cloud` | BLOW IT! | swipe | `cloud.png` 10개 + `fan.png` + reveal(sky/field/sun) |

미니게임 활성/비활성: `MINIGAME_TYPES` 배열에서 주석 처리.

## 에셋 목록
```
asset/sandbag.png
asset/glove_left.png, glove_right.png
asset/fist.svg, palm.svg              ← 인트로 제스처 힌트 아이콘
asset/bubble_00~06.png                (비활성)
asset/window_00~04.png, rag.png
asset/crumple_00~04.png               ← 종이 5단계
asset/trash.png, trash_open.png       ← 휴지통 닫힘/열림
asset/hand_open.png, hand_close.png   ← 구기기 중 제스처 표시
asset/swatter.png, fly.png
asset/cloud.png, fan.png, sky.png, field.png, sun.png
```

## Key JS 구조
```js
const S = { phase, warmupHits, warmupTarget, mgQueue, currentMG, ... }
const MG = { bubble, wipe, crumple, swat, cloud }

function gesture(raw)         // 'shake' | 'wave' | 'idle' (디바이스 프로토콜 그대로)
function initMinigame(type)   // 미니게임 초기화
function mgClear(type)        // 미니게임 클리어 → 다음으로
function hideMinigameUI(type) // 즉시 display:none
function startNextMinigame()  // MG_INTRO 플래시 + 제스처 힌트 → initMinigame
```

## 제스처 네이밍
- 디바이스 수신: `'shake'` / `'wave'` (프로토콜 불변)
- UI 표시: **SMASH** (shake) / **SWIPE** (wave)
- 인트로 힌트: SMASH = `fist.svg` 위아래 bounce / SWIPE = `palm.svg` 좌우 sweep
- `#mg-gesture-hint` z-index:60, 인트로 텍스트와 1.1초 함께 표시

## 키보드 (데모용)
```
1 → smash (shake)
2 → swipe (wave)
r → record (디바이스 연결 시)
```

## Web Serial
```js
port = await navigator.serial.requestPort();
await port.open({ baudRate: 115200 });
// 수신: 'shake' | 'wave' | 'idle'
// 무시: 'READY', 'START', 'END', 'Recording...'
// 전송: 'r'
```

## 규칙
- `transform`, `opacity`만 애니메이션. `top`/`left`/`width` 직접 애니메이션 금지.
- `hideMinigameUI()` 로만 숨김 (즉시, 타이머 지연 없음).
- `#typo-block` z-index: 25 / `#mg-intro` + `#mg-gesture-hint` z-index: 60 / HUD z-index: 30.
- box-shadow / border-radius 추가 금지 (명시 요청 없으면).
- `spawnParts(x, y, count)` 파티클 호출.

## 미니게임별 clear 흐름
- `wipe`: `showWipeClear()` → window_04 위에 "Spotless/CLEARED" 2.2초 오버레이 → 다음
- `cloud`: `showCloudReveal()` → sky 0ms → field 500ms → sun 900ms → 텍스트 2000ms → 종료 3600ms
- 나머지: `mgClear(type)` → `animImpact` → `startNextMinigame` 900ms
