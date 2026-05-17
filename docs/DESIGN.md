# SMASH — Design Specification (현재 상태)

## Design Philosophy

전면 스테이지. 미니게임 오브젝트들이 화면 전체를 채우고, HUD는 하단 floating pill.
왼쪽 패널 없음. 타이포그래피는 우상단 editorial 배치. 폭력적 인터랙션 + 차분한 여백의 대비.

---

## Color

```css
--ink:   #111110   /* 기본 텍스트, 오브젝트 */
--dim:   #8A8880   /* muted 라벨 */
--red:   #D4351C   /* shake 강조 */
--go:    #16A34A   /* 연결됨 상태 */
--paper: #FAFAF8   /* 서류 배경 */

Stage background: #FFFFFF
HUD background:   rgba(226,231,238,0.65)  + backdrop-filter:blur(14px)
Ghost text:       rgba(17,17,16,0.032)
```

그라디언트, 글로우, 네온 없음. 쉐도우/border-radius 추가 금지 (명시 요청 없으면).

---

## Typography

```css
font-family: 'Geist', sans-serif;              /* UI, body */
font-family: 'Geist Mono', monospace;          /* 라벨, 상태, 카운터 */
font-family: 'Playfair Display', serif italic; /* editorial 제목 */
```

### Scale
- 미니게임 지시문: `font-size:clamp(5rem,13vw,10rem)`, `font-weight:900`, `letter-spacing:-0.06em`
- Warmup 카운터: `font-size:clamp(6rem,18vw,14rem)`, `color:rgba(17,17,16,0.06)`
- Impact 단어: `font-size:clamp(5rem,15vw,12rem)`, `font-weight:900`
- Editorial 스크립트: `Playfair Display italic`, `clamp(2.2rem,4vw,3.4rem)`
- Editorial 볼드: `Geist 900`, `clamp(3.5rem,7vw,6rem)`, `letter-spacing:-0.05em`
- HUD 라벨: `Geist Mono 10px`, `letter-spacing:0.1em`, uppercase
- 카운터: `Geist Mono 13px`, `letter-spacing:0.22em`, uppercase

---

## Layout

### 전체 구조
```
[Full-screen Stage]
  ├── #canvas (z:0)  파티클
  ├── #ghost (z:0)   배경 텍스트 "STRESS"
  ├── 각 미니게임 레이어 (z:3~10)
  ├── #typo-block (z:25)  우상단 editorial
  ├── #cleared (z:25)     라운드 클리어
  ├── #mg-intro (z:60)    미니게임 지시문 플래시
  └── #hud-container (z:30) 하단 floating
```

왼쪽 패널 없음. 1280px 고정 viewport width.

### 우상단 Editorial Block (`#typo-block`)
- `position:absolute; top:5rem; right:2rem; text-align:right; z-index:25`
- Playfair italic + Geist 900 두 줄 조합
- 항상 미니게임 위에 보임

### 하단 HUD (`#hud-container`)
- `position:absolute; bottom:1.75rem; left:50%`
- Floating pill: 상태 dot + status 텍스트 + 구분선 + 연결 버튼
- 키보드 힌트 텍스트 아래에 표시

---

## 샌드백

```
#bag-wrap: top:0; left:50%; transform-origin:top center
#bag-cord: width:1px; height:60px; 그라디언트 #B0ACA4→#989088
#bag-img: height:clamp(420px,68dvh,820px)
          filter:drop-shadow(0 32px 64px rgba(17,17,16,0.16))
```

- 워밍업 단계에만 활성 (MG_ACTIVE 중에도 표시되지만 중심에서 벗어나 있음)
- shake → 큰 스윙 ±25deg 댐핑 진동
- wave → 중간 스윙 ±12deg
- idle → ±3deg 천천히 진동

---

## 미니게임 레이어 규칙

- 각 미니게임은 `display:none ↔ display:block/flex`로 토글
- `hideMinigameUI(type)`: 즉시 `display:none` (딜레이 없음)
- init 함수에서 opacity/transform 반드시 리셋
- z-index 범위: 미니게임 3~10, #typo-block 25, #mg-intro 60

---

## 미니게임별 비주얼

### WIPE IT — 창문 닦기 (wave)
```
#mg-wipe-bg: inset:0; z-index:6
#wipe-img: inset:5%; 90%×90%; object-fit:cover
  → window_00.png ~ window_04.png (wave마다 다음 프레임)
#rag-el: 420px wide; translateX(-500px) 시작 → 오른쪽으로 sweep
  → 4개 Y 위치 순환; sweep 중간(50%)에 이미지 교체
```

### CRUMPLE! — 종이 구기기 (shake)
```
#mg-crumple: flex; align-items:center; justify-content:center; z-index:3
#paper-el: SVG 서류; transform-origin:center; transition 0.22s
  → 4단계 변형: skew → 구겨짐 → 둥글게 → 튕겨나감
  → initCrumple에서 opacity:1 리셋 필수
```

### SWAT IT! — 파리채 (shake)
```
#mg-swat: inset:0; z-index:3
.fly: 72px×72px; position:absolute; left:0; top:0
  → transform만으로 이동 (left/top 직접 변경 금지)
  → 5개 파리, zone-based 배치 (뷰포트 5구역)
  → .fly.dead: opacity:0; transform:scale(0)
#swatter-el: 700px wide; transform-origin:top center
  → 상단에서 slam 애니메이션 (글러브와 동일 패턴)
```

### BLOW IT! — 구름 날리기 (wave)
```
#mg-cloud: inset:0; z-index:3; overflow:hidden
.cloud-el: 크기 clamp(60vw,70vw,85vw); position:absolute
  → center-based Y 배치: const y = centerY - cloudH/2
  → 3행 배치: centerY at 12%, 38%, 65% of vh
  → cloud.png 7개 배치
  → .blown: opacity:0; transform:translateX(±1200px)
#fan-el: 380px; bottom:8%; left:50%; transform-origin:bottom center
  → 부채질 애니메이션: -55deg → +55deg 왕복
reveal 순서:
  → 0ms: sky 페이드인
  → 1000ms: field translateY(105%→0)
  → 1600ms: sun translateY(-110%→0)
  → 4500ms: mgClear
```

### (비활성) BUBBLE — 뽁뽁이 (shake)
```
주석처리됨. MINIGAME_TYPES에서 제외.
bubble_00~06.png: 전체 화면 cover, shake마다 다음 프레임
```

---

## 애니메이션 규칙

- `transform`, `opacity`만 사용. `top`/`left`/`width` 직접 애니메이션 금지.
- 스크린 shake: `#stage.shake` CSS 애니메이션 0.18s
- Impact word: opacity 0→1 80ms, 400ms 후 fade out
- 파티클: `spawnParts(x, y, count)` → canvas에 렌더링

---

## WarioWare 지시문 플래시 (`#mg-intro`)

```css
position:absolute; top:46%; left:50%; z-index:60;
font-size:clamp(5rem,13vw,10rem); font-weight:900;
transform:translate(-50%,-50%) scale(0.75); opacity:0;
transition:opacity 0.1s, transform 0.15s cubic-bezier(0.16,1,0.3,1);
```
- `.show`: opacity:1, scale(1)
- 900ms 후 fade out → 미니게임 시작

---

## HUD 상태

| 상태 | dot | status 텍스트 |
|------|-----|--------------|
| 미연결 | gray `rgba(30,42,60,0.3)` | DISCONNECTED |
| 연결됨 | green `#16A34A` | CONNECTED |
| 녹화중 | green blink | RECORDING |

버튼: `Geist Mono 10px`, uppercase, 흰 배경 pill, `box-shadow:0 1px 3px rgba(0,0,0,0.08)`

---

## 데모용 키보드

```
1 → shake
2 → wave
3 → idle
r → record (연결 시)
```
