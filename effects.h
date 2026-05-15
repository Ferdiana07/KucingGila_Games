#ifndef GAME_EFFECTS_H
#define GAME_EFFECTS_H

#include "game_common.h"

// ============== FUNGSI PARTICLE SYSTEM ==============
// Spawn partikel baru di posisi (x,y,z) dengan warna RGB, jumlah count, dan kecepatan initial
void spawnParticles(float x, float y, float z, float r, float g, float b, int count, float speed);

// Update posisi dan lifetime semua partikel aktif setiap frame
void updateParticles();

// Render/draw semua partikel aktif ke layar (menggunakan blending untuk transparansi)
void drawParticles();

// ============== FUNGSI SCREEN FLASH EFFECT ==============
// Trigger kilatan layar dengan warna (r,g,b) dan intensitas (a/alpha) saat ada event penting
void triggerFlash(float r, float g, float b, float a);

// Update fade-out screen flash effect setiap frame
void updateFlash();

// ============== FUNGSI SHADOW RENDERING ==============
// Draw shadow blob di bawah karakter pada posisi (x,z) dengan radius tertentu
void drawBlobShadow(float x, float z, float radius);

// ============== FUNGSI TEXTURE LOADING ==============
// Load file BMP dan convert ke OpenGL texture format (return texture ID)
// Fungsi ini di-implement di effects.cpp
// GLuint loadBMPTexture(const char *filename);

#endif // GAME_EFFECTS_H
