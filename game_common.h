#ifndef GAME_COMMON_H
#define GAME_COMMON_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAP_ROWS 25
#define MAP_COLS 33
#define PI 3.14159265f
#define NUM_GHOSTS 3
#define MAX_PARTICLES 400
#define POWER_DURATION 420
#define STARTING_LIVES 3
#define WIN_COIN_TARGET 150

enum GameState
{
    START,
    PLAYING,
    GAME_OVER,
    WIN
};

typedef struct Particle
{
    float x, y, z;
    float vx, vy, vz;
    float r, g, b, a;
    float life, maxLife;
    float size;
    bool active;
} Particle;

typedef struct Ghost
{
    float x, z;
    float speed;
    float stuckTimer;
    float wanderAngle;
    bool active;
    int invincTimer;
    int type;
    bool eaten;
    int respawnTimer;
} Ghost;

extern int initial_maze[MAP_ROWS][MAP_COLS];
extern int maze[MAP_ROWS][MAP_COLS];
extern const float offsetX;
extern const float offsetZ;

extern GameState currentState;
extern Particle particles[MAX_PARTICLES];
extern float flashR, flashG, flashB, flashA;

extern float pX, pZ;
extern float pAngle;
extern float mouthAnim;
extern float mouthDir;
extern int lives;
extern int coins;
extern int totalCoins;
extern int coinTarget;
extern int score;
extern int highScore;
extern int scoreMultiplier;
extern int multTimer;
extern int ghostEatCombo;
extern bool powerActive;
extern int powerTimer;
extern bool nightmareActive;
extern int dashCooldown;
extern int dashTimer;

extern float camX;
extern float camY;
extern float camZ;
extern bool firstPersonMode;

extern Ghost ghosts[NUM_GHOSTS];

extern bool keys[256];
extern bool arrowKeys[4];
extern float mouseSensitivity;
extern bool mouseWarping;

extern float shakeMag;
extern float shakeDecay;
extern float dangerLevel;
extern int dangerParticleCooldown;
extern bool showJumpscare;
extern int jumpscareTimer;
extern int heavyJumpscareTimer;
extern int heavyJumpscareCooldown;
extern GLuint jumpscareTexture;

GLuint loadBMPTexture(const char *filename);

#endif
