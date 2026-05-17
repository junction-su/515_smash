# CLAUDE.md — SMASH

Single-file browser app. No build step. Vanilla HTML/CSS/JS + Canvas.

## Files
```
smash.html          ← 전체 게임 (HTML+CSS+JS 단일 파일)
asset/              ← 모든 PNG 에셋 (transparent bg)
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
샌드백 워밍업 (4~5 shake) → 미니게임 4개 랜덤 → ROUND CLEAR → 반복
```

게임 페이즈: `WARMUP` → `MG_INTRO` → `MG_ACTIVE` → `MG_CLEAR` → `ROUND_CLEAR`

## 미니게임 목록

| ID | 지시문 | 제스처 | 상태 |
|---|---|---|---|
| `bubble` | POP IT! | shake | **비활성** (주석처리) |
| `wipe` | WIPE IT! | wave | `window_00~04.png` + `rag.png` |
| `crumple` | CRUMPLE! | shake | SVG 서류 4단계 변형 |
| `swat` | SWAT IT! | shake | `fly.png` 5마리 + `swatter.png` |
| `cloud` | BLOW IT! | wave | `cloud.png` 7개 + `fan.png` + reveal |

미니게임 활성/비활성: `MINIGAME_TYPES` 배열에서 주석 처리.

## 에셋 목록
```
asset/sandbag.png
asset/glove_left.png, glove_right.png
asset/bubble_00~06.png   (비활성)
asset/window_00~04.png, rag.png
asset/swatter.png, fly.png
asset/cloud.png, fan.png, sky.png, field.png, sun.png
```

## Key JS 구조
```js
const S = { phase, warmupHits, warmupTarget, mgQueue, currentMG, ... }
const MG = { bubble, wipe, crumple, swat, cloud }  // 게임별 상태

function gesture(raw)        // 'shake' | 'wave' | 'idle' 처리
function initMinigame(type)  // 미니게임 초기화
function mgClear(type)       // 미니게임 클리어 → 다음으로
function hideMinigameUI(type)// 즉시 숨김 (display:none)
```

## 키보드 (데모용)
```
1 → shake
2 → wave
3 → idle
r → record (디바이스 연결 시)
```

## Web Serial
```js
port = await navigator.serial.requestPort();
await port.open({ baudRate: 115200 });
// 수신: 'shake' | 'wave' | 'idle' (줄바꿈으로 구분)
// 무시: 'READY', 'START', 'END', 'Recording...'
// 전송: 'r' (recording 트리거)
```

## 규칙
- `transform`, `opacity`만 애니메이션. `top`/`left`/`width` 직접 애니메이션 금지.
- 미니게임 UI는 `hideMinigameUI()` 로만 숨김 (타이머 지연 없이 즉시).
- `#typo-block` z-index: 25 (항상 미니게임 위에).
- 파티클: `spawnParts(x, y, count)` 호출.
- 디바이스 연결 없이 키보드로 전체 테스트 가능.
