#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "game_common.h"

// ============== FUNGSI MAP MANAGEMENT ==============
// Reset map ke state awal: copy dari initial_maze dan tempatkan semua koin
void resetMap();

// ============== FUNGSI QUERY MAP ==============
// Hitung total jumlah koin dalam map saat ini
int getTotalCoins();

// ============== FUNGSI COLLISION DETECTION ==============
// Check apakah cell di (row, col) adalah ruang kosong (bukan tembok)
bool isFreeCell(int row, int col);

// ============== FUNGSI COLLISION 3D ==============
// Check collision dengan dinding pada posisi (x, z) dalam koordinat world 3D
bool isWall(float x, float z);

// ============== FUNGSI COLLISION KAMERA ==============
// Check collision dengan dinding untuk kamera (memungkinkan kamera masuk tembok sedikit)
bool isWallCamera(float x, float z);

// ============== FUNGSI COLLECT ITEM ==============
// Pemain collect item pada posisi (x, z)
// Return: 0=tidak ada item, 1=koin biasa (+10 score), 2=power pellet (+50 score + power mode)
int collectItem(float x, float z);

// ============== FUNGSI RENDER MAP ==============
// Draw map 3D: semua dinding, koin biasa, dan power pellets
void drawMap();

#endif // GAME_MAP_H
