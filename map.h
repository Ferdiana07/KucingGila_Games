#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "game_common.h"

void resetMap();
int getTotalCoins();
bool isFreeCell(int row, int col);
bool isWall(float x, float z);
bool isWallCamera(float x, float z);
int collectItem(float x, float z);
void drawMap();

#endif
