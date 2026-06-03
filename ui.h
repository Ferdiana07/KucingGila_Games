#ifndef GAME_UI_H
#define GAME_UI_H

#include "game_common.h"

void renderStr(float x, float y, void *font, const char *s);
void drawFilledRect(float x1, float y1, float x2, float y2, float r, float g, float b, float a);
void drawHeart(float cx, float cy, float sz);
void drawMinimap(int w, int h);
void drawPowerBar(int w, int h);
void drawJumpscare(int w, int h);
void drawUI(int w, int h);
void display();
void updatePhysics(int value);
void mouseMotion(int mx, int my);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void reshape(int w, int h);

#endif
