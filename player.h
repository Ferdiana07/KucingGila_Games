#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "game_common.h"

// ============== FUNGSI LIGHTING SETUP ==============
// Setup 3 light sources (ambient, player light, ghost light) dan fog effect
// Menggunakan OpenGL fixed pipeline lighting untuk rendering 3D
void setupLighting();

// ============== FUNGSI PLAYER LIGHT UPDATE ==============
// Update light yang mengikuti pemain (dynamic lighting yang berubah saat power mode)
void updatePlayerLight();

// ============== FUNGSI FOG COLOR UPDATE ==============
// Update warna fog berdasarkan jarak terdekat ke hantu (red saat dekat hantu)
void updateFogColor(float minGhostDist);

// ============== FUNGSI RENDER KUCING ==============
// Draw 3D model kucing di posisi pemain dengan animasi mulut
void drawPacman(float mouthAngle);

// ============== FUNGSI AUDIO FEEDBACK ==============
// Play beep sound effect saat makan koin ("coin"), power ("power"), atau mati ("death")
void feedbackBeep(const char *type);

// ============== FUNGSI KAMERA CONTROL ==============
// Update posisi dan rotasi kamera follow dari belakang pemain setiap frame
void updateCamera();

// ============== FUNGSI RESET/INIT ==============
// Reset pemain ke posisi awal dan reset semua game variables
void resetPlayer();

// ============== FUNGSI INISIALISASI GAME ==============
// Initialize game baru: reset map, hantu, pemain, coins, score
void initGame();

#endif // GAME_PLAYER_H
