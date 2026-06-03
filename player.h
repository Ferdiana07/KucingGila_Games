#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "game_common.h"

void setupLighting();
void updatePlayerLight();
void updateFogColor(float minGhostDist);
void feedbackBeep(const char *type);
void drawPacman(float mouthAngle);
void updateCamera();
void resetPlayer();
void initGame();

#endif
