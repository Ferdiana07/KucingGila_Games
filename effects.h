#ifndef GAME_EFFECTS_H
#define GAME_EFFECTS_H

#include "game_common.h"

void spawnParticles(float x, float y, float z, float r, float g, float b, int count, float speed);
void updateParticles();
void drawParticles();
void triggerFlash(float r, float g, float b, float a);
void updateFlash();
void drawBlobShadow(float x, float z, float radius);
GLuint loadBMPTexture(const char *filename);

#endif
