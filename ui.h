#ifndef GAME_UI_H
#define GAME_UI_H

#include "game_common.h"

// ============== FUNGSI RENDERING TEXT ==============
// Render string text di posisi (x,y) menggunakan GLUT bitmap font
// font: GLUT_BITMAP_HELVETICA_18, GLUT_BITMAP_TIMES_ROMAN_24, dll
void renderStr(float x, float y, void *font, const char *s);

// ============== FUNGSI RENDERING SHAPE ==============
// Draw filled rectangle dari (x1,y1) sampai (x2,y2) dengan warna RGBA
// Menggunakan GL_BLEND untuk transparency (alpha)
void drawFilledRect(float x1, float y1, float x2, float y2, float r, float g, float b, float a);

// Draw 3D heart shape di posisi (cx,cy) dengan ukuran sz
// Digunakan untuk life indicator (3 hati = 3 nyawa)
void drawHeart(float cx, float cy, float sz);

// ============== FUNGSI UI ELEMENTS ==============
// Draw minimap (peta kecil) ukuran 148x112 pixel di pojok kanan atas
// Show player, hantu, dan maze layout dengan grid cells
void drawMinimap(int w, int h);

// Draw power pellet duration bar saat powerActive=true
// Show remaining duration dengan warna magenta dan pulsing effect
void drawPowerBar(int w, int h);

// Draw jumpscare image (saat hantu ketangkap pemain)
// Load texture jumpscare dari "jp.bmp" dan display fullscreen
void drawJumpscare(int w, int h);

// ============== MAIN UI RENDERING ==============
// Draw semua UI elements berdasarkan game state:
// - START: menu screen, instructions, high score
// - PLAYING: HUD (lives, score, coins, level, minimap, power bar)
// - GAME_OVER/WIN: end screen dengan restart instruction
void drawUI(int w, int h);

// ============== CALLBACK FUNCTIONS ==============

// GLUT display callback - dipanggil setiap frame untuk render
// Clear buffer, render 3D world (map, pemain, hantu), render UI
void display();

// GLUT timer callback - dipanggil setiap 16ms untuk update physics
// Update player movement, hantu AI, particles, camera, collision detection
void updatePhysics(int value);

// GLUT mouse motion callback - track mouse movement untuk kamera rotation
// mx, my = current mouse position
void mouseMotion(int mx, int my);

// GLUT keyboard callback - handle key press events
// key = ASCII code, x,y = mouse position
// W/S = maju/mundur, A/D = putar, ENTER = start/restart, ESC = exit
void keyboard(unsigned char key, int x, int y);

// GLUT keyboard up callback - handle key release
// Used untuk tracking which keys currently pressed (keys[] array)
void keyboardUp(unsigned char key, int x, int y);

// GLUT special key callback - handle arrow keys, function keys
// key = GLUT_KEY_LEFT/RIGHT/UP/DOWN
void specialKey(int key, int x, int y);

// GLUT special key up callback - handle arrow key release
void specialKeyUp(int key, int x, int y);

// GLUT reshape callback - dipanggil saat window di-resize
// Adjust viewport, projection matrix, dan minimap size berdasarkan w,h
void reshape(int w, int h);

#endif // GAME_UI_H
