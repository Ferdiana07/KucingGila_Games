# DOKUMENTASI KODE GAME KUCING GILA - UPGRADED EDITION

## 📋 RINGKASAN STRUKTUR FILE

### 1. game_common.h

**Fungsi**: Header file utama yang define semua konstanta, struct, dan extern variables
**Konstanta Utama**:

- MAP_ROWS = 25, MAP_COLS = 33 (ukuran grid labirin)
- NUM_GHOSTS = 3 (jumlah hantu)
- MAX_PARTICLES = 400 (max partikel effect)
- POWER_DURATION = 300 (frame durasi power pellet ~5 detik)

**Struktur Penting**:

- `Particle`: x,y,z posisi, vx,vy,vz velocity, RGBA color, life timer
- `Ghost`: x,z posisi, speed, type (0=red, 1=blue/smart, 2=pink), eaten flag, respawnTimer
- `GameState`: START, PLAYING, GAME_OVER, WIN

---

### 2. effects.cpp / effects.h

**Fungsi**: Visual effects seperti partikel, screen flash, shadow, dan texture loading

#### spawnParticles(x, y, z, r, g, b, count, speed)

- Spawn `count` partikel baru di posisi (x,y,z) dengan warna RGB
- Setiap partikel punya random direction (spherical coordinates) dan speed
- Partikel hidup 30-60 frames dengan fade-out effect
- Digunakan saat: makan koin, makan hantu, powerup, dll

#### updateParticles()

- Update posisi setiap partikel dengan velocity-nya
- Apply gravity: vy -= 0.003f
- Update alpha: a = life / maxLife (fade-out effect)
- Deaktifkan partikel saat life <= 0

#### drawParticles()

- Render semua partikel aktif sebagai glowing spheres
- Gunakan additive blending (GL_SRC_ALPHA, GL_ONE) untuk glow effect
- 4x4 resolution sphere untuk performa

#### triggerFlash() & updateFlash()

- Flash layar dengan warna RGB untuk event penting
- triggerFlash() set warna dan durasi (alpha)
- updateFlash() fade-out: alpha -= 0.04f per frame

#### drawBlobShadow(x, z, radius)

- Draw circular shadow di bawah karakter
- Digunakan untuk shadow kucing/player dan hantu
- 24-segment circle untuk smooth rendering
- Semi-transparent black (0,0,0,0.45f)

#### loadBMPTexture(filename)

- Load file BMP dan convert ke OpenGL texture
- Parse BMP header untuk width/height
- Swap BGR to RGB channels (BMP format issue)
- Return texture ID untuk glBindTexture()
- Used untuk jumpscare image

---

### 3. main.cpp

**Fungsi**: Entry point program, inisialisasi GLUT dan setup callbacks

#### main(argc, argv)

- Seed random dengan current time
- Initialize keyboard arrays dan particles
- Setup GLUT window (1024x720)
- Register callbacks:
  - **display**: render frame
  - **reshape**: handle window resize
  - **keyboard/specialKey**: input handling
  - **mouseMotion**: kamera control
  - **timer (16ms)**: physics update ~60 FPS

---

### 4. player.cpp / player.h

**Fungsi**: Logic pemain, lighting, kamera, dan render kucing

#### setupLighting()

- **3 Light Sources**:
  - LIGHT0 (Ambient): Global illumination sangat rendah agar labirin lebih gelap
  - LIGHT1 (Player Light): Follow pemain, berubah merah saat hantu dekat dan ungu saat power mode
  - LIGHT2 (Ghost Light): Lampu merah dari hantu terdekat saat danger
- **Fog**: Exponential fog lebih tebal untuk atmosphere. Saat hantu dekat, fog berubah merah gelap agar suasana lebih mencekam.
- Enable GL_COLOR_MATERIAL untuk dynamic lighting
- Enable GL_NORMALIZE untuk konsisten normal vector

#### updatePlayerLight()

- Update LIGHT1 position ke (pX, 1.6, pZ)
- Jika powerActive: warna light berubah ungu/magenta dengan pulsing
- Else: warna light kuning/oranye normal, lalu bergeser merah mengikuti `dangerLevel` saat hantu dekat
- Update LIGHT2 di posisi hantu terdekat saat `dangerLevel` aktif, dengan warna merah kuat sebagai cahaya ancaman
- Pulsing effect menggunakan sinf(time)

#### updateFogColor()

- Ubah suasana fog berdasarkan jarak terdekat ke hantu
- Jika hantu dekat: fog menjadi merah gelap untuk suasana horror/mencekam
- Jika hantu jauh: fog normal gelap
- Efek danger diperkuat oleh border layar, partikel merah, fog merah, dan lighting merah

#### drawPacman(mouthAngle)

- Draw 3D model kucing sebagai body, kepala, telinga, kaki, ekor, mata, dan hidung
- Body/fur color: putih krem terang agar karakter terlihat lebih jelas seperti kucing
- mouthAngle: kontrol buka/tutup mulut
- Rendered dengan glutSolidSphere()

#### updateCamera()

- Mendukung 2 perspektif kamera:
  - Third person: kamera follow dari belakang pemain, dengan collision kamera terhadap tembok
  - First person: kamera berada di posisi mata kucing/player agar labirin terasa lebih imersif
- Tombol `C` toggle antara first person dan third person
- Use gluLookAt untuk point kamera ke depan pemain

#### resetPlayer()

- Reset pemain ke posisi awal (pX=0, pZ=8)
- Reset angle (pAngle=180)
- Reset nyawa (lives=3), score, coins
- Reset camera

#### initGame()

- Initialize game baru
- Reset map dan hantu
- Reset pemain dan UI
- Set currentState = PLAYING

---

### 5. ghost.cpp / ghost.h

**Fungsi**: Ghost AI logic dan pathfinding

#### spawnGhostRandom(idx)

- Spawn hantu di random posisi yang jauh dari pemain (min 6 unit)
- Cek isFreeCell() untuk posisi valid
- Retry hingga 600 kali untuk jamin posisi bagus

#### updateGhost(idx)

- **Hantu dimakan (eaten=true)**:
  - Tunggu respawn timer (frames)
  - Respawn: set eaten=false, invincTimer=120
  - Spawn partikel warna sesuai type hantu
- **Invincibility**: Jika invincTimer > 0, jangan bisa dimakan
- **Normal behavior** (powerActive=false):
  - Hitung direction ke pemain: (dx, dz) normalize
  - **Type 0 (Red)**: Chase langsung
  - **Type 1 (Blue)**: Smart chasing - ambil posisi depan pemain (3 unit di depan)
  - **Type 2 (Pink)**: Chase normal
  - Obstacle avoidance: jika tersandung dinding, wandering
- **Power mode** (powerActive=true):
  - Hantu takut! Lari dari pemain
  - Direction reverse: nx = -nx, nz = -nz
  - Speed 60% lebih lambat
  - Hantu dapat dimakan saat power

#### drawGhost(frightened, scale, ghostIndex, frightenTimer)

- Draw 3D model hantu
- Jika frightened: warna biru gelap
- Else: warna type hantu dibuat lebih gelap/seram
- Mata dibuat merah menyala, ditambah alis tajam, mulut hitam, taring, dan spike kepala
- Scale: ukuran hantu
- Use glutSolidSphere() untuk badan

---

### 6. map.cpp / map.h

**Fungsi**: Map/labirin management, collision, dan item collection

#### resetMap()

- Copy initial_maze ke maze array (reset layout)
- Place semua koin biasa (value 2 di map)
- Place semua power pellets (value 3 di map)

#### getTotalCoins()

- Count total koin biasa dalam maze
- Loop seluruh grid, count cell == 2

#### isFreeCell(row, col)

- Check apakah cell (row,col) adalah space kosong (bukan tembok)
- Return true jika maze[row][col] != 1

#### isWall(x, z)

- Check collision 3D dengan dinding di posisi world (x,z)
- Convert world coords ke grid coords
- Check boundary dan cell value

#### isWallCamera(x, z)

- Seperti isWall tapi lebih permissive untuk kamera
- Allow kamera masuk tembok 0.2 unit

#### collectItem(x, z)

- Cek ada item di posisi (x,z)
- Return 0: tidak ada, 1: koin (+10 score), 2: power (+50 score + power mode)
- Update maze array saat item dikumpulkan

#### drawMap()

- Render semua dinding (tembok), koin, dan power pellets
- Dinding: 3D cube dengan satu tema warna biru/cyan gelap
- Tembok memakai `GL_EMISSION` kecil supaya pola bata tetap terlihat di area gelap.
- Jika terlihat ada cahaya di dinding, itu berasal dari emission material tembok, retakan glow cyan, koin/power pellet, atau danger lighting.
- Texture procedural:
  - `drawWallTextureOverlay()`: pola bata, garis mortar, dan retakan glow
  - `drawFloorTextureOverlay()`: garis tile, jejak kaki kucing, fishbone kecil
  - `drawCeilingTextureOverlay()`: sparkle/rune cyan halus di atap
- Koin biasa: small sphere warna cyan
- Power pellet: larger sphere warna magenta

---

### 7. ui.cpp / ui.h

**Fungsi**: UI rendering - menu, HUD, minimap, score

#### drawUI(w, h)

- Render semua UI elements

**START state**:

- Background panel semi-transparent black
- Title "KUCING GILA - UPGRADED EDITION" dengan pulsing effect
- Instructions tentang kontrol
- High score display (jika ada)

**PLAYING state**:

- **3 Hearts**: Life indicator (kiri atas)
- **Coins**: "Koin: X/Y" (kanan atas, tengah)
- **Score**: "Skor: Z" (kanan atas, bawah)
- **Minimap**: 148x112 pixel, pojok kanan atas
  - Show maze layout dengan warna berbeda per cell type
  - Yellow dot: pemain posisi
  - Warna dots: hantu posisi (1 per hantu)
- **Power Bar**: durasi power pellet saat aktif
- **Combo Multiplier**: "x2 COMBO!" saat makan hantu
- **Danger Overlay**: border merah berdenyut saat hantu dekat

**GAME_OVER / WIN state**:

- Tampilkan game over/victory message
- Render high score
- Instruksi untuk restart

---

## 📊 GAME FLOW DIAGRAM

```
START
  ↓
[Menu Screen - Tekan ENTER]
  ↓
PLAYING
  ├─ Update:
  │  ├─ Player movement (W/S/A/D)
  │  ├─ Camera follow (mouse)
  │  ├─ Hantu AI pathfinding
  │  ├─ Collision detection
  │  ├─ Item collection
  │  ├─ Power pellet effects
  │  ├─ Particle effects
  │  └─ Screen shake
  │
  ├─ Render:
  │  ├─ 3D World (map, hantu, kucing/player, particles, shadows)
  │  ├─ HUD (score, lives, minimap)
  │  └─ Light effects (fog, flash, lighting)
  │
  └─ Check Win/Loss:
     ├─ Ketangkap hantu → GAME_OVER
     ├─ Kumpulkan semua koin → WIN
     └─ Else → tetap PLAYING
      ↓
[Game Over / Victory Screen]
      ↓
[Tekan R untuk main lagi → kembali ke PLAYING]
```

---

## 🎮 KONTROL GAME

| Tombol | Fungsi                   |
| ------ | ------------------------ |
| W      | Maju                     |
| S      | Mundur                   |
| A      | Putar kiri               |
| D      | Putar kanan              |
| SPACE  | Dash cepat dengan cooldown |
| Mouse  | Rotasi kamera kiri/kanan |
| C      | Toggle First Person / Third Person |
| ENTER  | Mulai game               |
| R      | Main lagi saat Game Over / Win |
| ESC    | Exit                     |

---

## 🎯 GAME MECHANICS

### Scoring

- Koin biasa: +10 points
- Power pellet: +50 points
- Makan hantu (saat power): +200, +400, +800 (multiplier)

### Power Pellet

- Durasi: 300 frames (~5 detik)
- Effect: Hantu jadi takut dan berwarna biru
- Pemain bisa makan hantu dan dapat combo multiplier
- Light effect: ungu/magenta pulsing

### Hantu AI

- Type 0 (Red): Simple chase
- Type 1 (Blue): Smart chase ke posisi depan pemain
- Type 2 (Pink): Standard chase
- Saat power: lari dari pemain dengan speed 60%
- Nightmare Chase: aktif saat sisa koin sedikit; hantu lebih cepat dan danger zone lebih besar

### Dash dan Jumpscare

- `SPACE`: dash cepat selama beberapa frame, lalu masuk cooldown
- Saat hantu sangat dekat, `heavyJumpscareTimer` memunculkan jumpscare berat, flash merah, dan camera shake
- Semua suara dimatikan; `feedbackBeep()` sengaja menjadi no-op

### First-Person Horror

- Tombol `C` toggle kamera first person / third person
- Saat first person, FOV lebih sempit, fog lebih tebal, model kucing disembunyikan, dan minimap dimatikan

### Lighting

- 3 light sources untuk atmosphere
- Fog exponential untuk depth effect
- Player light berubah saat power mode
- Saat hantu dekat, suasana berubah lewat fog merah gelap, border layar, partikel merah, dan player light merah
- Lantai dan atap ikut terkena bias merah karena lighting dunia sengaja dibuat mencekam saat danger
- Tembok punya emission pulsing kecil dan retakan glow agar texture tetap terbaca di labirin gelap.

---

## 💡 TIPS PENGEMBANGAN

### Untuk mengatur difficulty:

1. Edit `POWER_DURATION` di game_common.h (kurangi durasi power)
2. Edit `speedBase` di `initGame()` pada player.cpp
3. Edit ghost respawn timer (lebih cepat respawn)

### Untuk menambah karakter hantu:

1. Edit `NUM_GHOSTS` di game_common.h
2. Tambahkan instance baru di ghosts array
3. Tambahkan type dan warna di drawGhost()

### Untuk menambah map:

1. Edit `initial_maze` di map.cpp
2. 0 = kosong, 1 = tembok, 2 = koin, 3 = power pellet

---

## 📝 CATATAN TEKNIS

- **Language**: C++ dengan OpenGL (GLUT)
- **Graphics**: Fixed pipeline OpenGL dengan 3 light sources
- **Audio**: Dimatikan; feedback game memakai visual effect saja
- **Particle System**: Max 400 particles aktif simultan
- **Target FPS**: 60 (16ms per frame)
