# Crazy Cat 3D

Crazy Cat 3D adalah game labirin 3D berbasis OpenGL/GLUT. Pemain mengendalikan kucing di dalam labirin gelap, mengumpulkan koin, memakai power pellet, dan menghindari tiga hantu yang mengejar.

## Fitur

- Labirin 3D dengan lantai, tembok, atap, glow, dan fog.
- Model kucing, hantu, koin ikan, power pellet, partikel, dan efek jumpscare.
- Tiga hantu dengan perilaku berbeda.
- Mode power untuk memakan hantu dan mendapatkan combo score.
- Kamera third person dan first person.
- HUD berisi nyawa, skor, jumlah koin, minimap, dan status dash.
- Layar awal, game over, dan win screen.

## Build

Pastikan compiler C++ dan FreeGLUT sudah tersedia, lalu jalankan:

```bash
g++ -o kucing_gila main.cpp effects.cpp ghost.cpp map.cpp player.cpp ui.cpp \
    -lfreeglut -lopengl32 -lglu32
```

Di Windows, jalankan executable dari folder project agar file `jp.bmp` dapat ditemukan.

## Kontrol

| Tombol | Fungsi |
| --- | --- |
| `W/S` atau `Up/Down` | Maju dan mundur |
| `A/D` atau `Left/Right` | Putar kiri dan kanan |
| Mouse | Lihat sekitar |
| `C` | Ganti kamera |
| `Space` | Dash |
| `Enter` | Mulai game |
| `R` | Main lagi setelah kalah/menang |
| `Esc` | Keluar |

## Aturan

- Kumpulkan 150 koin untuk menang.
- Hindari hantu saat power mode tidak aktif.
- Ambil power pellet ungu untuk membuat hantu bisa dimakan sementara.
- Makan beberapa hantu saat power mode untuk menaikkan combo score.

## Struktur File

| File | Isi |
| --- | --- |
| `main.cpp` | Inisialisasi GLUT dan callback utama |
| `game_common.h` | Konstanta, struct, dan state global |
| `map.cpp/.h` | Data labirin, collision, item, dan render map |
| `player.cpp/.h` | State pemain, kamera, lighting, dan model kucing |
| `ghost.cpp/.h` | Spawn, AI, dan render hantu |
| `effects.cpp/.h` | Partikel, flash, shadow, dan texture BMP |
| `ui.cpp/.h` | HUD, layar awal, game over, win screen, dan input |
| `jp.bmp` | Texture jumpscare |
