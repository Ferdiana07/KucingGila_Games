#ifndef GAME_GHOST_H
#define GAME_GHOST_H

#include "game_common.h"

void spawnGhostRandom(int idx);
void updateGhost(int idx);
void drawGhost(bool frightened, float scale, int ghostIndex, float frightenTimer);

#endif
