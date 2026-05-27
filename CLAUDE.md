# SMASH — Project Context

## 프로젝트 개요
웨어러블 너클 디바이스(손가락 마디에 끼는 버튼 장치)로 조작하는 WarioWare식 스트레스 해소 미니게임 모음. 단일 HTML 파일 (`index.html`).

## 기술 스택
- Vanilla JS + CSS (프레임워크 없음)
- Web Serial API (너클 디바이스 연결, Chrome/Edge 전용)
- 단일 파일: `index.html` (CSS + JS 인라인)

## 제스처 → 게임 액션
| 디바이스 신호 | 게임 액션 | 키보드 |
|---|---|---|
| `shake` | SMASH (주먹 클렌치) | `1` |
| `wave` | SWIPE (손 흔들기) | `2` |
| `idle` | 아무것도 안 함 | `3` |

## 게임 페이즈
`S.phase`: `TUTORIAL` → `WARMUP` → `MG_INTRO` → `MG_ACTIVE` → `MG_CLEAR` → `ROUND_CLEAR` → (반복)

## 미니게임 목록
`MINIGAME_TYPES = ['wipe', 'crumple', 'swat', 'cloud']`

| 게임 | 제스처 | 핵심 함수 |
|---|---|---|
| wipe (창문 닦기) | wave | `handleWipeWave()` |
| crumple (종이 구기기) | shake | `handleCrumpleShake()` |
| swat (파리 잡기) | shake | `handleSwatShake()` |
| cloud (먹구름 날리기) | wave | `showFanSweep()` |

## 에셋 구조
```
asset/
  sfx/          ← 효과음 (mp3, 48kbps/22kHz)
    bag_punch.mp3
    swat_fly.mp3   (loop, 3s trimmed)
    swat_hit.mp3
    cloud_wind.mp3
    wipe_squeak.mp3
    crumple_paper.mp3
    crumple_trash.mp3
  *.webp         ← 이미지 (PNG에서 WebP 변환, 1920px max)
  knuckle_open.webp / knuckle_fist.webp  ← 튜토리얼 코치용
```

## CSS 변수 (`:root`)
```css
--ink:   #111110   /* 메인 텍스트/테두리 */
--paper: #FAFAF8   /* 배경 */
--dim:   #888880   /* 서브 텍스트 */
```
폰트: `Geist` (900 헤드라인) + `Geist Mono` (서브/모노)

## SFX 시스템
```js
getSFX(key)    // lazy load + preload='auto' + .load()
playSFX(key)   // currentTime=0 후 play
stopSFX(key)   // pause + reset
warmupSFX()    // 첫 제스처 시 호출 — 모든 오디오 silent play-pause로 디코딩
```
**주의**: `warmupSFX()`는 keydown + touch 버튼 첫 클릭 시 호출. 각 `initXxx()` 함수에서 `getSFX()` 프리로드.

## 튜토리얼 (데스크탑 전용)
- 모바일 `@media (hover:none)` → `display:none`
- `S.phase = 'TUTORIAL'` → `gesture()` 최상단에서 `tutorialHit()` 라우팅
- STEP1: shake → `animateGlove()` + fist 확정
- STEP2: wave → 샌드백 `tut-sway` + `cloud_wind` SFX
- 완료: "LET'S GO!" 플래시 → `exitIntro()` → WARMUP

## 브랜치
`claude/build-smash-html-PzfLq`
