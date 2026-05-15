# 🎮 KUCING GILA - UPGRADED EDITION

Game 3D Pacman-like berbasis OpenGL dengan kucing putih krem, 3 hantu AI, labirin gelap, texture procedural, dan efek bahaya saat hantu mendekat.

## 📁 STRUKTUR FILE DAN KOMENTAR

Semua file di project ini sudah dilengkapi dengan komentar detail yang menjelaskan fungsi setiap blok kode:

### 📄 Header Files (dengan penjelasan fungsi)

- **`game_common.h`** - Konstanta game, struct (Particle, Ghost, GameState), extern variables
  - Komentar: Define, typedef, extern dengan penjelasan tujuan setiap variable
- **`effects.h`** - Fungsi untuk particle effects, flash, shadow, texture loading
  - Komentar: Setiap fungsi dijelaskan inputnya dan kegunaannya

- **`player.h`** - Fungsi untuk lighting, kamera, render kucing/player
  - Komentar: Detail setiap fungsi lighting dan rendering

- **`ghost.h`** - Fungsi untuk AI hantu dan pathfinding
  - Komentar: Spawn, update, dan draw hantu dijelaskan

- **`map.h`** - Fungsi untuk map management dan collision detection
  - Komentar: Reset map, query collision, collect item dijelaskan

### 📚 Implementation Files (dengan penjelasan detail)

- **`main.cpp`** ✅ - Entry point program, GLUT initialization
  - Komentar: Lengkap untuk setiap GLUT callback registration
- **`effects.cpp`** ✅ - Particle system, flash effects, texture loading
  - Komentar: Setiap fungsi dijelaskan line-by-line (physics, blending, texture format)
- **`player.cpp`** ⚡ - Lighting, kamera, render kucing/player
  - Komentar: setupLighting() dan updatePlayerLight() sudah lengkap dijelaskan
  - Detail: 3 light sources, fog gelap, attenuation, kucing putih, power mode effects
- **`ghost.cpp`** - Ghost AI logic
  - Fungsi: spawnGhostRandom(), updateGhost(), drawGhost()
  - AI strategy: simple chase, smart chase, power mode
- **`map.cpp`** - Map layout, collision, item collection
  - Detail: Maze grid, collision detection, item spawn points, texture procedural tembok/lantai/atap
- **`ui.cpp`** - UI rendering (menu, HUD, minimap, score)
  - Fungsi: renderStr(), drawText(), drawMinimap(), drawUI(), danger overlay

### 📖 DOKUMENTASI LENGKAP

- **`CODE_DOCUMENTATION.md`** - Dokumentasi komprehensif semua fungsi utama
  - Game flow diagram
  - Kontrol game
  - Game mechanics (scoring, power pellet, hantu AI)
  - Lighting system explanation
  - Tips untuk development/modifikasi

---

## 🎯 PANDUAN MEMBACA KODE

### Untuk Pemula (ingin cepat paham)

1. Baca `CODE_DOCUMENTATION.md` bagian "RINGKASAN STRUKTUR FILE"
2. Lihat file headers (.h) untuk memahami struktur data
3. Baca `main.cpp` untuk flow program
4. Baca `effects.cpp` untuk contoh implementasi dengan komentar detail

### Untuk Programmer (ingin detail)

1. Mulai dari `game_common.h` - lihat semua define dan struct
2. Baca `main.cpp` - understand GLUT callbacks
3. Baca `effects.cpp` - lihat implementation dengan komentar detail
4. Baca `player.cpp` setupLighting() - lihat lighting system
5. Baca `ghost.cpp` - pahami AI pathfinding logic
6. Baca `ui.cpp` - lihat rendering dan minimap implementation

### Untuk Modifier (ingin ubah/tambah fitur)

1. Lihat `CODE_DOCUMENTATION.md` bagian "TIPS PENGEMBANGAN"
2. Untuk ubah difficulty: edit ghost speed, power duration di header files
3. Untuk ubah map: edit `initial_maze` di `map.cpp`
4. Untuk ubah kontrol/UI: edit `main.cpp` callbacks atau `ui.cpp`

---

## 🔍 PENJELASAN KOMENTAR DALAM KODE

Semua file menggunakan format komentar standar:

```cpp
// ============== NAMA BLOK/FUNGSI ==============
// Penjelasan singkat apa yang dilakukan
//
// Detail penjelasan untuk baris-baris penting
//
for (int i = 0; i < MAX; i++) {
    // Komentar untuk baris kompleks
    variabel = value;  // Inline comment untuk clarity
}
```

Komentar menjelaskan:

- **What**: Apa fungsi ini lakukan
- **Why**: Mengapa menggunakan cara ini
- **How**: Bagaimana implementation-nya

---

## 🛠️ KOMPILASI & RUN

**Compile Manual:**

```bash
g++ -o kucing_gila main.cpp effects.cpp ghost.cpp map.cpp player.cpp ui.cpp \
    -lfreeglut -lopengl32 -lglu32
```

**Jalankan:**

```bash
./kucing_gila.exe
```

---

## 🎮 KONTROL GAME

| Tombol  | Fungsi          |
| ------- | --------------- |
| `W`     | Maju            |
| `S`     | Mundur          |
| `A`     | Putar kiri      |
| `D`     | Putar kanan     |
| `SPACE` | Dash cepat dengan cooldown |
| `Mouse` | Rotasi kamera   |
| `C`     | Toggle First Person / Third Person |
| `ENTER` | Mulai           |
| `R`     | Main lagi saat Game Over / Win |
| `ESC`   | Keluar          |

---

## 💡 HIGHLIGHT FITUR KODE

### 1. **3 Light Source System**

- **LIGHT0**: Ambient global sangat rendah agar labirin lebih gelap
- **LIGHT1**: Dynamic player light (follow pemain, berubah saat power)
- **LIGHT2**: Lampu merah dari hantu terdekat saat danger
- **Fog**: Exponential fog lebih tebal untuk depth + atmosphere. Saat hantu dekat, fog dan pencahayaan berubah merah gelap supaya suasana terasa mencekam.
- **Kenapa tembok terlihat bercahaya?** Material tembok sengaja memakai `GL_EMISSION` kecil dan retakan glow cyan agar pola bata tetap kelihatan di labirin gelap. Glow ini efek visual, bukan lampu baru.

**Lokasi:** `player.cpp` - `setupLighting()`, `updatePlayerLight()`

### 1b. **Procedural Texture & Dekor**

- Tembok punya pola bata, garis mortar, dan retakan glow kecil.
- Lantai punya garis tile, jejak kaki kucing, dan fishbone kecil.
- Atap punya sparkle/rune cyan halus.
- Semua dibuat langsung dengan primitive OpenGL, jadi tidak perlu tambah file gambar.

**Lokasi:** `map.cpp` - `drawWallTextureOverlay()`, `drawFloorTextureOverlay()`, `drawCeilingTextureOverlay()`

### 2. **Particle Effect System**

- Max 400 partikel simultan
- Physics: gravity, velocity, fade-out
- Rendering: additive blending untuk glow
- Used untuk: koin collection, ghost eaten, power activation

**Lokasi:** `effects.cpp` - `spawnParticles()`, `updateParticles()`, `drawParticles()`

### 3. **Ghost AI Pathfinding**

- Type 0 (Red): Simple chase
- Type 1 (Blue): Smart chase + predict player position
- Type 2 (Pink): Standard chase + wandering stuck detection
- **Nightmare Chase**: aktif saat sisa koin sedikit; hantu lebih cepat dan suasana makin merah/mencekam

**Lokasi:** `ghost.cpp` - `updateGhost()`

### 4. **Minimap Rendering**

- 148x112 pixel minimap di pojok kanan atas
- Show player position (yellow), hantu (colored dots), maze layout
- Cell types: wall (dark), path, coin, power pellet
- Saat First Person, minimap disembunyikan agar mode horror terasa lebih total.

**Lokasi:** `ui.cpp` - `drawMinimap()`

### 5. **Collision Detection**

- Grid-based collision system
- Player vs wall, vs items, vs ghosts
- Camera vs wall (permissive untuk better view)

**Lokasi:** `map.cpp` - `isWall()`, `isWallCamera()`, `collectItem()`

### 6. **Dash, Nightmare, dan Jumpscare**

- `SPACE` memberi dash cepat sebentar dengan cooldown.
- Nightmare Chase aktif otomatis ketika koin hampir habis.
- Jika hantu sangat dekat, muncul jumpscare berat, flash merah, dan camera shake.
- Semua efek suara dimatikan; feedback dibuat visual saja.

**Lokasi:** `ui.cpp` - `updatePhysics()`, `drawHeavyJumpscare()`

---

## 📊 GAME STATISTICS

| Elemen            | Nilai                        |
| ----------------- | ---------------------------- |
| Map size          | 25×33 grid                   |
| Light sources     | 3 (ambient, player, ghost)   |
| Max active ghosts | 3                            |
| Max particles     | 400                          |
| Power duration    | 300 frames (~5 sec @ 60 FPS) |
| Level system      | Tidak ada; menang berarti game selesai |
| Target FPS        | 60                           |
| Window size       | 1024×720 pixels              |

---

## 🔧 DEVELOPMENT TIPS

### Untuk menambah kesulitan:

```cpp
// game_common.h
#define POWER_DURATION 200  // Kurangi dari 300 (lebih cepat power habis)

// ghost.cpp - di updateGhost()
g.speed *= 1.3f;  // Hantu lebih cepat
```

### Untuk menambah hantu (4 hantu):

```cpp
// game_common.h
#define NUM_GHOSTS 4

// Tambah type baru di drawGhost() untuk warna ke-4
```

### Untuk mengubah map layout:

```cpp
// map.cpp - initial_maze[][]
// 0 = kosong, 1 = wall, 2 = koin, 3 = power pellet
```

---

## 📚 STRUKTUR ALUR KODE

```
main()
  ↓
GLUT initialization
  ↓
setupLighting() → 3 light sources + fog
  ↓
glutMainLoop()
  ↓
[Per Frame]
  ├─ display() → render 3D world + UI
  ├─ reshape() → handle window resize
  ├─ keyboard/specialKey() → input
  ├─ updatePhysics() → update logic
  │  ├─ updateParticles()
  │  ├─ updatePlayerLight()
  │  ├─ updateFogColor()
  │  ├─ updateCamera()
  │  └─ updateGhost(i) × 3 hantu
  ├─ collectItem() → check koin collection
  ├─ isWall() → collision detection
  └─ drawUI() → render menu/HUD
```

---

## ✅ CHECKLIST FILE DENGAN KOMENTAR

- [x] game_common.h - Semua define, struct, extern dijelaskan
- [x] effects.h - Semua fungsi dijelaskan
- [x] player.h - Semua fungsi dijelaskan
- [x] ghost.h - Semua fungsi dijelaskan
- [x] map.h - Semua fungsi dijelaskan
- [x] main.cpp - Semua initialization dijelaskan
- [x] effects.cpp - Setiap fungsi + logika dijelaskan detail
- [x] player.cpp - setupLighting() dan updatePlayerLight() dijelaskan detail
- [✓] ghost.cpp - AI logic + fungsi dijelaskan
- [✓] map.cpp - Grid, collision, item collection dijelaskan
- [✓] ui.cpp - UI rendering dijelaskan

---

## 🎓 SUMBER BELAJAR

Buat mengerti setiap bagian kode:

1. Baca header file dulu untuk struktur
2. Baca main() untuk flow
3. Baca implementation files untuk detail
4. Compile dan jalankan, ubah nilai untuk eksperimen
5. Baca CODE_DOCUMENTATION.md untuk penjelasan mendalam

---

**Last Updated:** May 15, 2026  
**Language:** C++ dengan OpenGL (GLUT)  
**Status:** Complete dengan komentar detail ✅
