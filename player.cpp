#include "player.h"
#include "map.h"
#include "ghost.h"
#include "effects.h"
#include <math.h>
#include <string.h>

GameState currentState = START;

float pX = 0.f;
float pZ = 8.f;
float pAngle = 180.f;
float mouthAnim = 0.f;
float mouthDir = 1.f;
int lives = 3;
int coins = 0;
int totalCoins = 0;
int score = 0;
int highScore = 0;
int scoreMultiplier = 1;
int multTimer = 0;
int ghostEatCombo = 0;
bool powerActive = false;
int powerTimer = 0;
int level = 1;

float camX = 0.f;
float camY = 1.6f;
float camZ = 12.f;

bool keys[256];
bool arrowKeys[4];
float mouseSensitivity = 0.20f;
bool mouseWarping = false;

float shakeMag = 0.0f;
float shakeDecay = 0.82f;

bool showJumpscare = false;
int jumpscareTimer = 0;
GLuint jumpscareTexture = 0;

// ============== SETUP LIGHTING ==============
// Fungsi: Initialize 3 light sources dan fog untuk game atmosphere
void setupLighting()
{
    // Enable lighting system dan 3 light sources
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Ambient light (global illumination)
    glEnable(GL_LIGHT1); // Player light (dynamic, follow pemain)
    glEnable(GL_LIGHT2); // Ghost light (bonus effect)

    // Enable depth testing untuk 3D rendering
    glEnable(GL_DEPTH_TEST);

    // Enable color material tracking - allow dynamic color changes
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // Apply color ke ambient+diffuse

    // Enable normal normalization untuk konsisten lighting
    glEnable(GL_NORMALIZE);

    // ============== SETUP LIGHT 0 (AMBIENT) ==============
    // Ambient light: constant global illumination (tidak ada shadow)
    GLfloat a0[] = {0.03f, 0.03f, 0.05f, 1}; // Warna ambient gelap biru
    GLfloat d0[] = {0, 0, 0, 1};             // Diffuse nol (no directional light)
    GLfloat p0[] = {0, 50, 0, 1};            // Posisi tinggi di atas map
    glLightfv(GL_LIGHT0, GL_AMBIENT, a0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, d0);
    glLightfv(GL_LIGHT0, GL_POSITION, p0);

    // ============== SETUP FOG ==============
    // Fog effect untuk depth perception dan atmosphere
    glEnable(GL_FOG);
    GLfloat fogCol[] = {0.02f, 0.02f, 0.04f, 1}; // Fog warna gelap biru
    glFogfv(GL_FOG_COLOR, fogCol);
    glFogi(GL_FOG_MODE, GL_EXP2);   // Exponential fog (smooth fade)
    glFogf(GL_FOG_DENSITY, 0.12f);  // Fog density (0.12 = medium dense)
    glHint(GL_FOG_HINT, GL_NICEST); // Quality hint

    // Set background color sama dengan fog color
    glClearColor(0.02f, 0.02f, 0.04f, 1);
}

// ============== UPDATE PLAYER LIGHT ==============
// Fungsi: Update dynamic light yang follow pemain, berubah saat power mode
void updatePlayerLight()
{
    // Posisi light di pemain (1.6 = height mata)
    GLfloat pos[] = {pX, 1.6f, pZ, 1};
    GLfloat amb[4], dif[4];

    // ============== POWER MODE - PURPLE LIGHT ==============
    if (powerActive)
    {
        // Pulsing effect: 0.6 sampai 1.0 range dengan sinusoidal
        float pulse = 0.6f + 0.4f * sinf(glutGet(GLUT_ELAPSED_TIME) / 60.0f);

        // Ambient: mostly blue/purple sedikit merah
        amb[0] = 0.05f;
        amb[1] = 0.0f;
        amb[2] = 0.1f * pulse;
        amb[3] = 1;

        // Diffuse: strong magenta dengan pulsing
        dif[0] = 0.4f * pulse;
        dif[1] = 0.1f;
        dif[2] = 0.9f * pulse;
        dif[3] = 1;
    }
    // ============== NORMAL MODE - YELLOW LIGHT ==============
    else
    {
        // Ambient: warm yellowish tone
        amb[0] = 0.06f;
        amb[1] = 0.04f;
        amb[2] = 0.01f;
        amb[3] = 1;

        // Diffuse: strong yellow/orange normal mode
        dif[0] = 0.70f;
        dif[1] = 0.50f;
        dif[2] = 0.20f;
        dif[3] = 1;
    }

    // Apply light properties
    glLightfv(GL_LIGHT1, GL_POSITION, pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);

    // Attenuation: light fade dengan distance (realistic)
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);   // Base = 1.0
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.20f);    // Linear = 0.2*d
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.07f); // Quadratic = 0.07*d^2
}

void updateFogColor(float minGhostDist)
{
    float danger = 1.0f - fminf(1.0f, minGhostDist / 6.0f);
    if (powerActive)
        danger = 0;
    float fr = 0.02f + danger * 0.18f;
    float fg = 0.02f * (1 - danger);
    float fb = powerActive ? (0.04f + 0.1f * sinf(glutGet(GLUT_ELAPSED_TIME) / 200.0f)) : 0.04f * (1 - danger * 0.5f);
    GLfloat fogCol[] = {fr, fg, fb, 1};
    glFogfv(GL_FOG_COLOR, fogCol);
    glClearColor(fr * 0.5f, fg * 0.5f, fb * 0.5f, 1);
}

void feedbackBeep(const char *type)
{
    if (strcmp(type, "coin") == 0)
    {
        Beep(900, 35);
    }
    else if (strcmp(type, "power") == 0)
    {
        Beep(400, 60);
        Beep(600, 60);
        Beep(800, 80);
    }
    else if (strcmp(type, "eat") == 0)
    {
        Beep(700, 50);
        Beep(900, 80);
    }
    else if (strcmp(type, "hit") == 0)
    {
        Beep(220, 100);
        Beep(160, 130);
    }
    else if (strcmp(type, "win") == 0)
    {
        Beep(523, 80);
        Beep(659, 80);
        Beep(784, 200);
    }
    else if (strcmp(type, "lose") == 0)
    {
        Beep(300, 150);
        Beep(200, 220);
    }
}

void drawPacman(float mouthAngle)
{
    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    glPushMatrix();
    GLfloat fur_amb[] = {0.15f, 0.08f, 0.02f, 1};
    GLfloat fur_diff[] = {0.55f, 0.35f, 0.15f, 1};
    GLfloat fur_spec[] = {0.4f, 0.3f, 0.2f, 1};
    GLfloat no_emi[] = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, fur_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, fur_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, fur_spec);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emi);
    glMaterialf(GL_FRONT, GL_SHININESS, 30);
    glPushMatrix();
    glScalef(0.7f, 0.55f, 0.9f);
    glutSolidSphere(0.38f, 18, 18);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 0.22f, 0.30f);
    glScalef(1.0f, 0.9f, 0.85f);
    glutSolidSphere(0.22f, 16, 16);
    glPopMatrix();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.11f, 0.42f, 0.28f);
    glRotatef(10, 0, 0, 1);
    glRotatef(-15, 1, 0, 0);
    glScalef(0.6f, 1.0f, 0.5f);
    glutSolidCone(0.08f, 0.16f, 8, 4);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.11f, 0.42f, 0.28f);
    glRotatef(-10, 0, 0, 1);
    glRotatef(-15, 1, 0, 0);
    glScalef(0.6f, 1.0f, 0.5f);
    glutSolidCone(0.08f, 0.16f, 8, 4);
    glPopMatrix();
    GLfloat ear_amb[] = {0.3f, 0.1f, 0.1f, 1};
    GLfloat ear_diff[] = {0.8f, 0.4f, 0.45f, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, ear_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ear_diff);
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.11f, 0.43f, 0.30f);
        glRotatef(s * -10, 0, 0, 1);
        glRotatef(-10, 1, 0, 0);
        glScalef(0.35f, 0.6f, 0.3f);
        glutSolidCone(0.07f, 0.12f, 6, 3);
        glPopMatrix();
    }
    glMaterialfv(GL_FRONT, GL_AMBIENT, fur_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, fur_diff);
    for (int tv = 0; tv < 8; tv++)
    {
        float frac = tv / 7.0f;
        float sw = sinf(t * 3.5f + frac * 2.5f) * 0.15f;
        float tx = sw;
        float ty = frac * 0.30f;
        float tz = -0.30f - frac * 0.22f;
        float r2 = 0.055f - frac * 0.005f;
        glPushMatrix();
        glTranslatef(tx, ty, tz);
        glutSolidSphere(r2, 8, 8);
        glPopMatrix();
    }
    float legPos[4][2] = {{-0.14f, 0.15f}, {0.14f, 0.15f}, {-0.12f, -0.15f}, {0.12f, -0.15f}};
    for (int l = 0; l < 4; l++)
    {
        float legBob = (l < 2 ? 0 : PI) * 0.5f;
        float dy = -0.02f * sinf(t * 8.0f + legBob);
        glPushMatrix();
        glTranslatef(legPos[l][0], -0.18f + dy, legPos[l][1]);
        glRotatef(90, 1, 0, 0);
        GLUquadric *lq = gluNewQuadric();
        gluCylinder(lq, 0.04f, 0.045f, 0.16f, 8, 3);
        gluDeleteQuadric(lq);
        glPopMatrix();
    }
    GLfloat eye_amb[] = {0.1f, 0.3f, 0, 1};
    GLfloat eye_diff[] = {0.3f, 0.9f, 0.1f, 1};
    GLfloat eye_emi[] = {0.1f, 0.5f, 0, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, eye_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, eye_diff);
    glMaterialfv(GL_FRONT, GL_EMISSION, eye_emi);
    glMaterialf(GL_FRONT, GL_SHININESS, 90);
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.08f, 0.27f, 0.48f);
        glScalef(1.0f, 1.3f, 1.0f);
        glutSolidSphere(0.04f, 10, 10);
        glPopMatrix();
    }
    GLfloat pup_amb[] = {0, 0, 0, 1}, pup_diff[] = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, pup_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pup_diff);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emi);
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.08f, 0.27f, 0.505f);
        glScalef(0.3f, 1.2f, 0.3f);
        glutSolidSphere(0.025f, 6, 6);
        glPopMatrix();
    }
    GLfloat nose_amb[] = {0.3f, 0.1f, 0.1f, 1};
    GLfloat nose_diff[] = {0.9f, 0.4f, 0.5f, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, nose_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, nose_diff);
    glPushMatrix();
    glTranslatef(0, 0.22f, 0.51f);
    glScalef(1.0f, 0.8f, 0.5f);
    glutSolidSphere(0.025f, 8, 8);
    glPopMatrix();
}

void updateCamera()
{
    float rad = pAngle * PI / 180.f;
    float dirX = sinf(rad), dirZ = cosf(rad);
    float wantDist = 2.0f, camHgt = 1.5f, bestDist = wantDist;
    for (float d = wantDist; d > 0.25f; d -= 0.07f)
    {
        float cx2 = pX - dirX * d;
        float cz2 = pZ - dirZ * d;
        if (!isWallCamera(cx2, cz2))
        {
            bestDist = d;
            break;
        }
        bestDist = 0.25f;
    }
    float wantCamX = pX - dirX * bestDist;
    float wantCamZ = pZ - dirZ * bestDist;
    float wantCamY = camHgt;
    camX += (wantCamX - camX) * 0.18f;
    camY += (wantCamY - camY) * 0.18f;
    camZ += (wantCamZ - camZ) * 0.18f;
    if (shakeMag > 0.001f)
    {
        camX += ((float)rand() / RAND_MAX - 0.5f) * shakeMag;
        camY += ((float)rand() / RAND_MAX - 0.5f) * shakeMag * 0.4f;
        camZ += ((float)rand() / RAND_MAX - 0.5f) * shakeMag;
    }
}

void resetPlayer()
{
    pX = 0.f;
    pZ = 8.f;
    pAngle = 180.f;
    mouthAnim = 0.f;
    mouthDir = 1.f;
    float rad = pAngle * PI / 180.f;
    camX = pX - sinf(rad) * 2.f;
    camY = 1.5f;
    camZ = pZ - cosf(rad) * 2.f;
}

void initGame()
{
    resetMap();
    totalCoins = getTotalCoins();
    coins = 0;
    lives = 3;
    score = 0;
    scoreMultiplier = 1;
    multTimer = 0;
    ghostEatCombo = 0;
    powerActive = false;
    powerTimer = 0;
    resetPlayer();
    memset(particles, 0, sizeof(particles));
    shakeMag = 0;
    flashA = 0;
    float speedBase = 0.038f + level * 0.004f;
    for (int i = 0; i < NUM_GHOSTS; i++)
    {
        ghosts[i].speed = speedBase + i * 0.010f;
        ghosts[i].stuckTimer = 0;
        ghosts[i].wanderAngle = (float)(rand() % 360);
        ghosts[i].active = false;
        ghosts[i].invincTimer = 90 + i * 30;
        ghosts[i].type = i % 3;
        ghosts[i].eaten = false;
        ghosts[i].respawnTimer = 0;
        spawnGhostRandom(i);
    }
}
