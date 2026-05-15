#ifndef GAME_GHOST_H
#define GAME_GHOST_H

#include "game_common.h"

// ============== FUNGSI GHOST AI ==============
// Spawn hantu random di posisi yang jauh dari pemain (minimal jarak 6 unit)
// idx = indeks hantu (0, 1, 2)
void spawnGhostRandom(int idx);

// ============== FUNGSI UPDATE GHOST AI ==============
// Update AI logic hantu ke-i setiap frame:
// - Jika dimakan: menunggu respawn dan invincibility timer
// - Jika power aktif: hantu takut dan lari dari pemain
// - Normal: hantu mengejar pemain dengan simple pathfinding
// - Type 1 (biru): hantu cerdas yang ambil posisi depan pemain
void updateGhost(int idx);

// ============== FUNGSI RENDER GHOST ==============
// Draw 3D model hantu dengan texture/color berdasarkan type
// frightened = mode power (hantu biru takut)
// scale = ukuran hantu
// ghostIndex = indeks hantu (untuk warna berbeda)
// frightenTimer = durasi tersisa mode takut
void drawGhost(bool frightened, float scale, int ghostIndex, float frightenTimer);

#endif // GAME_GHOST_H
