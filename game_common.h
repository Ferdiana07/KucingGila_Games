#ifndef GAME_COMMON_H
#define GAME_COMMON_H

// ============== INCLUDE LIBRARY DAN HEADER ==============
// Untuk Windows, include library Windows untuk kompatibilitas
#ifdef _WIN32
#include <windows.h>
#endif

// Include OpenGL/GLUT untuk rendering 3D
#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h>

// ============== KONSTANTA GLOBAL ==============
#define MAP_ROWS 25        // Jumlah baris pada peta labirin (grid height)
#define MAP_COLS 33        // Jumlah kolom pada peta labirin (grid width)
#define PI 3.14159265f     // Nilai PI untuk perhitungan sudut (radian)
#define NUM_GHOSTS 3       // Jumlah hantu yang mengejar pemain (3 hantu)
#define MAX_PARTICLES 400  // Maksimal jumlah partikel visual effect (ledakan, dll)
#define POWER_DURATION 420  // Durasi power pellet dalam frame (420 frame = sekitar 7 detik)
#define STARTING_LIVES 3    // Nyawa awal pemain
#define WIN_COIN_TARGET 150 // Jumlah pelet/koin yang dibutuhkan untuk menang

// ============== ENUM STATUS GAME ==============
// Enum untuk tracking state game (mulai, bermain, game over, menang)
enum GameState
{
    START,     // State awal - layar menu
    PLAYING,   // State permainan sedang berlangsung
    GAME_OVER, // State pemain kalah (ketangkap hantu)
    WIN        // State pemain menang (semua koin terkumpul)
};

// ============== STRUCT PARTIKEL ==============
// Struktur untuk particle effect (ledakan, efek visual saat makan hantu/koin)
typedef struct Particle
{
    float x, y, z;       // Posisi partikel dalam ruang 3D
    float vx, vy, vz;    // Velocity/kecepatan partikel (gerak per frame)
    float r, g, b, a;    // Warna RGBA (Red, Green, Blue, Alpha/transparency)
    float life, maxLife; // Sisa hidup dan durasi maksimal partikel
    float size;          // Ukuran visual partikel (radius bola)
    bool active;         // Flag apakah partikel aktif/sedang ditampilkan
} Particle;

// ============== STRUCT HANTU (GHOST AI) ==============
// Struktur data untuk setiap hantu yang mengejar pemain
typedef struct Ghost
{
    float x, z;        // Posisi hantu di map (x=kolom, z=baris)
    float speed;       // Kecepatan pergerakan hantu per frame
    float stuckTimer;  // Timer untuk mendeteksi hantu terperangkap di sudut
    float wanderAngle; // Sudut untuk gerakan random saat bingung
    bool active;       // Flag apakah hantu aktif di game
    int invincTimer;   // Timer kebal hantu setelah di-respawn (tidak bisa dimakan)
    int type;          // Tipe hantu (0=merah, 1=biru/cerdas, 2=pink)
    bool eaten;        // Flag apakah hantu telah dimakan (dalam mode power)
    int respawnTimer;  // Timer untuk respawn hantu setelah dimakan
} Ghost;

// ============== EXTERN VARIABEL MAP ==============
extern int initial_maze[MAP_ROWS][MAP_COLS]; // Map awal (template labirin)
extern int maze[MAP_ROWS][MAP_COLS];         // Map saat ini (untuk tracking koin yang sudah dikumpulkan)
extern const float offsetX;                  // Offset untuk center peta di sumbu X
extern const float offsetZ;                  // Offset untuk center peta di sumbu Z

// ============== EXTERN VARIABEL GAME STATE ==============
extern GameState currentState;               // Status game saat ini (START/PLAYING/GAME_OVER/WIN)
extern Particle particles[MAX_PARTICLES];    // Array semua partikel yang aktif
extern float flashR, flashG, flashB, flashA; // Color flash effect (kilatan layar)

// ============== EXTERN VARIABEL PEMAIN ==============
extern float pX, pZ;        // Posisi pemain (X=kolom, Z=baris)
extern float pAngle;        // Arah hadap pemain dalam derajat (0-360)
extern float mouthAnim;     // Animasi mulut kucing/player (buka/tutup)
extern float mouthDir;      // Arah animasi mulut (1 atau -1)
extern int lives;           // Jumlah nyawa pemain
extern int coins;           // Jumlah koin yang sudah dikumpulkan
extern int totalCoins;      // Total koin dalam map
extern int coinTarget;      // Target koin yang harus dikumpulkan untuk menang
extern int score;           // Score pemain saat ini
extern int highScore;       // High score tertinggi
extern int scoreMultiplier; // Multiplier score saat makan hantu (x2, x4, dll)
extern int multTimer;       // Timer untuk reset multiplier
extern int ghostEatCombo;   // Combo hantu yang dimakan berturut-turut
extern bool powerActive;    // Flag apakah mode power pellet aktif
extern int powerTimer;      // Sisa waktu power pellet
extern bool nightmareActive; // Mode endgame: hantu lebih agresif saat koin hampir habis
extern int dashCooldown;     // Cooldown dash pemain dalam frame
extern int dashTimer;        // Durasi dash aktif dalam frame
// ============== EXTERN VARIABEL KAMERA ==============
extern float camX; // Posisi kamera X (lateral)
extern float camY; // Posisi kamera Y (ketinggian mata pemain ≈ 1.6)
extern float camZ; // Posisi kamera Z (jarak dari pemain)
extern bool firstPersonMode; // Toggle perspektif kamera: false=third person, true=first person

// ============== EXTERN VARIABEL HANTU ==============
extern Ghost ghosts[NUM_GHOSTS]; // Array 3 hantu

// ============== EXTERN VARIABEL INPUT ==============
extern bool keys[256];         // Array tombol keyboard (A-Z, dll)
extern bool arrowKeys[4];      // Array arrow keys (UP/DOWN/LEFT/RIGHT)
extern float mouseSensitivity; // Sensitivitas rotasi kamera dengan mouse
extern bool mouseWarping;      // Flag untuk disable mouse centering

// ============== EXTERN VARIABEL EFEK ==============
extern float shakeMag;          // Magnitude screen shake (guncangan layar)
extern float shakeDecay;        // Decay rate untuk screen shake effect
extern float dangerLevel;        // Intensitas bahaya saat hantu dekat (0.0 aman, 1.0 sangat dekat)
extern int dangerParticleCooldown; // Timer kecil agar partikel bahaya tidak spawn terlalu sering
extern bool showJumpscare;      // Flag menampilkan jumpscare image
extern int jumpscareTimer;      // Timer durasi jumpscare
extern int heavyJumpscareTimer; // Timer jumpscare berat saat hantu sangat dekat
extern int heavyJumpscareCooldown; // Cooldown agar jumpscare dekat tidak spam
extern GLuint jumpscareTexture; // Handle texture jumpscare (texture ID)

// ============== FUNGSI HELPER ==============
// Fungsi untuk load dan parse image BMP sebagai OpenGL texture
GLuint loadBMPTexture(const char *filename);

#endif // GAME_COMMON_H
