# SMASH — Design Specification (현재 상태)

## Design Philosophy

전면 스테이지. 미니게임 오브젝트들이 화면 전체를 채우고, HUD는 하단 floating pill.
왼쪽 패널 없음. 타이포그래피는 우상단 editorial 배치.

---

## Color

```css
--ink:   #111110   /* 기본 텍스트 */
--dim:   #8A8880   /* muted 라벨 */
--red:   #D4351C   /* smash 강조 */
--go:    #16A34A   /* 연결됨 */
--paper: #FAFAF8   /* 서류 배경 */

Stage background: #FFFFFF
HUD background:   rgba(226,231,238,0.65) + backdrop-filter:blur(14px)
Ghost text:       rgba(17,17,16,0.032)
```

box-shadow / border-radius / 그라디언트 추가 금지 (명시 요청 없으면).

---

## Typography

```css
'Geist', sans-serif          /* UI, body */
'Geist Mono', monospace      /* 라벨, 상태, 카운터 */
'Playfair Display', italic   /* editorial 제목 */
```

- 미니게임 지시문: `clamp(5rem,13vw,10rem)` weight 900
- 제스처 힌트 라벨: `Geist Mono 11px`, `letter-spacing:0.28em`
- HUD 라벨: `Geist Mono 10px`, `letter-spacing:0.1em`
- 카운터: `Geist Mono 13px`, `letter-spacing:0.22em`

---

## Layout

```
[Full-screen Stage #FFFFFF]
  ├── #canvas (z:0)            파티클
  ├── #ghost (z:0)             배경 텍스트
  ├── 미니게임 레이어 (z:3~10)
  ├── #typo-block (z:25)       우상단 editorial
  ├── #cleared (z:25)          라운드/미니게임 클리어
  ├── #mg-intro (z:60)         지시문 플래시
  ├── #mg-gesture-hint (z:60)  제스처 힌트 SVG 애니메이션
  └── #hud-container (z:30)    하단 floating pill
```

viewport: `width=1280` 고정.

---

## 샌드백

```
#bag-wrap: top:0; left:50%; transform-origin:top center
#bag-cord: 1px 높이 60px 그라디언트
#bag-img: height:clamp(420px,68dvh,820px)
```

---

## 미니게임 인트로 플래시 + 제스처 힌트

```
#mg-intro: top:42%, 큰 지시문 텍스트 (scale 0.75→1 팡! 등장)
#mg-gesture-hint: top:58%, 인트로 아래에 SVG 아이콘 + 라벨
```

- SMASH 게임: `fist.svg` + `@keyframes smash-hint` (위아래 bounce 0.38s)
- SWIPE 게임: `palm.svg` + `@keyframes swipe-hint` (좌우 sweep 0.85s)
- 1.1초 후 인트로와 함께 사라짐

---

## 미니게임별 비주얼

### WIPE IT — 창문 닦기 (swipe)
```
#mg-wipe-bg: inset:0; z-index:6
#wipe-img: inset:5%; 90%×90%; object-fit:cover
  → window_00~04.png (swipe마다 다음 프레임)
#rag-el: 420px; sweep 중간(50%)에 이미지 교체
완료: window_04 위에 "Spotless/CLEARED" 2.2초 오버레이
```

### CRUMPLE! — 종이 구기기 (smash)
```
crumple_00~04.png: 화면 중앙, smash마다 다음 단계
hand_open.png: 대기 상태 손 표시
hand_close.png: smash 시 순간 교체
crumple_04 완료 후: trash.png 오른쪽 중하단 등장
→ trash_open.png로 교체 → 구겨진 종이가 휴지통으로 슉 담김
```

### SWAT IT! — 파리채 (smash)
```
.fly: 72px, 5마리 zone-based 배치, transform만으로 이동
#swatter-el: 700px; 상단 slam 애니메이션
명중: 70% 확률 타겟팅 + ±140px 오차
```

### BLOW IT! — 구름 날리기 (swipe)
```
10개 구름, 3행 섹터 배치 (상3 중4 하3):
  row0: bottom-edge 앵커 (화면 위로 삐져나옴)
  row1: top 16~38%
  row2: top 60~76%
#fan-el: bottom:22%; 부채질 애니메이션 ±55deg
하단 흰색 그라디언트: rgba(255,255,255,0.5) z-index:9
reveal: sky 0ms → field 500ms → sun 900ms
→ 텍스트 오버레이 2000ms → 종료 3600ms
```

---

## WarioWare 지시문 플래시

```css
#mg-intro: position absolute; top:42%; z-index:60
transition: opacity 0.1s, transform 0.15s cubic-bezier(0.16,1,0.3,1)
.show: opacity:1; scale(1)
```

---

## HUD (하단 Floating Pill)

```
position:absolute; bottom:1.75rem; left:50%
background: rgba(226,231,238,0.65); backdrop-filter:blur(14px)
dot + status 텍스트 + 구분선 + 연결 버튼
keyboard hint: "keyboard: 1 smash · 2 swipe"
```

| 상태 | dot |
|------|-----|
| 미연결 | rgba(30,42,60,0.3) |
| 연결됨 | #16A34A |
| 녹화중 | #16A34A blink |
