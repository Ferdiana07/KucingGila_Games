#include "ghost.h"
#include "map.h"
#include "effects.h"
#include <math.h>

Ghost ghosts[NUM_GHOSTS];

void spawnGhostRandom(int idx)
{
    for (int attempt = 0; attempt < 600; attempt++)
    {
        int row = rand() % MAP_ROWS;
        int col = rand() % MAP_COLS;

        if (!isFreeCell(row, col))
            continue;

        float wx = col - (MAP_COLS - 1) * 0.5f;
        float wz = row - (MAP_ROWS - 1) * 0.5f;
        float dx = wx - pX;
        float dz = wz - pZ;

        if (sqrtf(dx * dx + dz * dz) > 6.f)
        {
            ghosts[idx].x = wx;
            ghosts[idx].z = wz;
            ghosts[idx].active = true;
            ghosts[idx].eaten = false;
            return;
        }
    }

    ghosts[idx].x = (float)(idx * 4 - 4);
    ghosts[idx].z = -5.f;
    ghosts[idx].active = true;
    ghosts[idx].eaten = false;
}

void updateGhost(int i)
{
    Ghost &g = ghosts[i];
    if (!g.active)
        return;

    if (g.eaten)
    {
        g.respawnTimer--;
        if (g.respawnTimer <= 0)
        {
            g.eaten = false;
            g.invincTimer = 120;
            spawnGhostRandom(i);
            spawnParticles(g.x, 1.0f, g.z,
                           g.type == 0 ? 0.9f : (g.type == 1 ? 0.3f : 0.6f),
                           g.type == 0 ? 0.1f : (g.type == 1 ? 0.9f : 0.1f),
                           g.type == 0 ? 0.1f : (g.type == 1 ? 0.3f : 0.9f),
                           20, 0.05f);
        }
        return;
    }

    if (g.invincTimer > 0)
    {
        g.invincTimer--;
        return;
    }

    float dx = pX - g.x;
    float dz = pZ - g.z;
    float dist = sqrtf(dx * dx + dz * dz);
    float nx = 0, nz = 0;

    if (dist > 0.01f)
    {
        nx = dx / dist;
        nz = dz / dist;
    }

    if (powerActive)
    {
        nx = -nx;
        nz = -nz;
    }

    if (g.type == 1 && !powerActive && dist > 1.5f)
    {
        float playerRad = pAngle * PI / 180.f;
        float tx = pX + sinf(playerRad) * 3.0f;
        float tz = pZ + cosf(playerRad) * 3.0f;
        float fdx = tx - g.x;
        float fdz = tz - g.z;
        float fd = sqrtf(fdx * fdx + fdz * fdz);

        if (fd > 0.01f)
        {
            nx = fdx / fd;
            nz = fdz / fd;
        }
    }

    float spd = g.speed * (powerActive ? 0.6f : (nightmareActive ? 1.20f : 1.0f));
    float gx2 = g.x + nx * spd;
    float gz2 = g.z + nz * spd;
    bool hx = isWall(gx2, g.z);
    bool hz = isWall(g.x, gz2);

    if (!hx)
        g.x = gx2;
    if (!hz)
        g.z = gz2;

    if (hx && hz)
    {
        g.stuckTimer++;
        if (g.stuckTimer > 8)
        {
            g.wanderAngle += 90.f * (float)(1 + rand() % 3);
            g.stuckTimer = 0;
        }

        float wr = g.wanderAngle * PI / 180.f;
        float wx2 = g.x + sinf(wr) * spd;
        float wz2 = g.z + cosf(wr) * spd;

        if (!isWall(wx2, g.z))
            g.x = wx2;
        if (!isWall(g.x, wz2))
            g.z = wz2;
    }
    else
    {
        g.stuckTimer = 0;
    }
}

void drawGhost(bool frightened, float scale, int ghostIndex, float frightenTimer)
{
    glPushMatrix();
    glScalef(scale, scale, scale);

    GLfloat bodyColors[3][3] = {
        {0.48f, 0.02f, 0.02f},
        {0.08f, 0.34f, 0.12f},
        {0.24f, 0.00f, 0.36f},
    };

    int idx = ghostIndex % 3;
    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    GLfloat body_diff[4], body_amb[4];

    if (frightened)
    {
        float blink = (frightenTimer < 90 && (int)(frightenTimer / 10) % 2 == 0) ? 0.8f : 1.0f;
        body_diff[0] = 0.05f * blink;
        body_diff[1] = 0.05f * blink;
        body_diff[2] = 0.9f * blink;
        body_diff[3] = 1;
        body_amb[0] = 0.02f;
        body_amb[1] = 0.02f;
        body_amb[2] = 0.3f;
        body_amb[3] = 1;
    }
    else
    {
        body_diff[0] = bodyColors[idx][0];
        body_diff[1] = bodyColors[idx][1];
        body_diff[2] = bodyColors[idx][2];
        body_diff[3] = 1;
        body_amb[0] = bodyColors[idx][0] * 0.18f;
        body_amb[1] = bodyColors[idx][1] * 0.18f;
        body_amb[2] = bodyColors[idx][2] * 0.18f;
        body_amb[3] = 1;
    }

    GLfloat body_spec[] = {0.45f, 0.45f, 0.45f, 1};
    GLfloat body_emi[] = {0, 0, 0, 1};
    if (frightened)
        body_emi[2] = 0.25f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, body_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, body_spec);
    glMaterialfv(GL_FRONT, GL_EMISSION, body_emi);
    glMaterialf(GL_FRONT, GL_SHININESS, 60);

    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadric *q = gluNewQuadric();
    gluCylinder(q, 1.0, 1.0, 1.5, 24, 8);
    gluDeleteQuadric(q);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 1.5f, 0);
    glutSolidSphere(1.0f, 24, 24);
    glPopMatrix();

    for (int b = 0; b < 5; b++)
    {
        float bx = -1.0f + b * 0.5f;
        float bob = sinf(t * 4.0f + b * 1.2f) * 0.05f;

        glPushMatrix();
        glTranslatef(bx, -0.3f + bob, 0);
        glScalef(0.22f, 0.35f, 0.22f);
        glutSolidSphere(1.0f, 10, 10);
        glPopMatrix();
    }

    GLfloat eye_diff[] = {1.0f, 0.05f, 0.02f, 1};
    GLfloat eye_amb[] = {0.35f, 0.0f, 0.0f, 1};
    GLfloat eye_emi[] = {0.55f, 0.0f, 0.0f, 1};
    GLfloat noEmi[] = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, eye_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, eye_diff);
    glMaterialfv(GL_FRONT, GL_EMISSION, eye_emi);
    glMaterialf(GL_FRONT, GL_SHININESS, 80);

    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.38f, 1.55f, 0.88f);
        glScalef(1.15f, 0.72f, 0.85f);
        glutSolidSphere(0.28f, 16, 16);
        glPopMatrix();
    }

    GLfloat pupil_diff[] = {0, 0, 0, 1};
    GLfloat pupil_amb[] = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, pupil_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pupil_diff);

    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.38f, 1.55f, 1.14f);
        glScalef(0.65f, 1.35f, 0.65f);
        glutSolidSphere(0.12f, 12, 12);
        glPopMatrix();
    }

    GLfloat brow_diff[] = {0.02f, 0.0f, 0.0f, 1};
    GLfloat brow_amb[] = {0.01f, 0.0f, 0.0f, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, brow_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, brow_diff);
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmi);
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.38f, 1.83f, 1.02f);
        glRotatef(s * -22.0f, 0, 0, 1);
        glScalef(0.38f, 0.06f, 0.06f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    GLfloat mouth_diff[] = {0.0f, 0.0f, 0.0f, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mouth_diff);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mouth_diff);
    glPushMatrix();
    glTranslatef(0.0f, 1.16f, 1.02f);
    glScalef(0.55f, 0.18f, 0.08f);
    glutSolidSphere(1.0f, 14, 8);
    glPopMatrix();

    GLfloat fang_diff[] = {0.95f, 0.92f, 0.82f, 1};
    GLfloat fang_amb[] = {0.25f, 0.22f, 0.18f, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, fang_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, fang_diff);
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.17f, 1.05f, 1.11f);
        glRotatef(90, 1, 0, 0);
        glutSolidCone(0.055f, 0.20f, 8, 4);
        glPopMatrix();
    }

    GLfloat spike_diff[] = {0.08f, 0.0f, 0.0f, 1};
    GLfloat spike_amb[] = {0.02f, 0.0f, 0.0f, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, spike_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, spike_diff);
    for (int s = -1; s <= 1; s++)
    {
        glPushMatrix();
        glTranslatef(s * 0.28f, 2.25f - fabsf((float)s) * 0.08f, 0.20f);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(0.12f, 0.34f, 8, 4);
        glPopMatrix();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPopMatrix();
}
