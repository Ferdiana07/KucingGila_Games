#include "ui.h"
#include "map.h"
#include "effects.h"
#include "player.h"
#include "ghost.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void renderStr(float x, float y, void *font, const char *s)
{
    glRasterPos2f(x, y);
    for (const char *c = s; *c; c++)
        glutBitmapCharacter(font, *c);
}

void drawFilledRect(float x1, float y1, float x2, float y2, float r, float g, float b, float a)
{
    glColor4f(r, g, b, a);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
    glDisable(GL_BLEND);
}

void drawHeart(float cx, float cy, float sz)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy - sz * 0.6f);
    for (int a = 0; a <= 180; a += 8)
    {
        float rad = a * PI / 180.f;
        glVertex2f(cx - sz * 0.5f + sz * 0.5f * cosf(rad), cy - sz * 0.3f + sz * 0.5f * sinf(rad));
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy - sz * 0.6f);
    for (int a = 0; a <= 180; a += 8)
    {
        float rad = a * PI / 180.f;
        glVertex2f(cx + sz * 0.5f * 0.4f + sz * 0.5f * cosf(rad), cy - sz * 0.3f + sz * 0.5f * sinf(rad));
    }
    glEnd();
}

void drawMinimap(int w, int h)
{
    float mmX = w - 160.0f;
    float mmY = 10.0f;
    float mmW = 148.0f;
    float mmH = (float)MAP_ROWS / MAP_COLS * mmW;
    float cellW = mmW / MAP_COLS;
    float cellH = mmH / MAP_ROWS;
    drawFilledRect(mmX - 2, mmY - 2, mmX + mmW + 2, mmY + mmH + 2, 0, 0, 0, 0.7f);
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            float cx = mmX + j * cellW;
            float cy = mmY + i * cellH;
            if (maze[i][j] == 1)
                glColor4f(0.1f, 0.1f, 0.3f, 0.9f);
            else if (maze[i][j] == 2)
                glColor4f(0.3f, 0.7f, 1.0f, 0.8f);
            else if (maze[i][j] == 3)
                glColor4f(0.9f, 0.1f, 0.9f, 0.9f);
            else
                glColor4f(0.04f, 0.04f, 0.08f, 0.7f);
            glBegin(GL_QUADS);
            glVertex2f(cx, cy);
            glVertex2f(cx + cellW, cy);
            glVertex2f(cx + cellW, cy + cellH);
            glVertex2f(cx, cy + cellH);
            glEnd();
        }
    }
    float px2 = mmX + (pX + offsetX) * cellW;
    float pz2 = mmY + (pZ + offsetZ) * cellH;
    glColor4f(1.0f, 0.85f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(px2 - 2, pz2 - 2);
    glVertex2f(px2 + 2, pz2 - 2);
    glVertex2f(px2 + 2, pz2 + 2);
    glVertex2f(px2 - 2, pz2 + 2);
    glEnd();
    for (int i = 0; i < NUM_GHOSTS; i++)
    {
        if (!ghosts[i].active || ghosts[i].eaten)
            continue;
        float gx2 = mmX + (ghosts[i].x + offsetX) * cellW;
        float gz2 = mmY + (ghosts[i].z + offsetZ) * cellH;
        if (powerActive)
            glColor4f(0.3f, 0.3f, 1.0f, 1.0f);
        else
        {
            float gc[3][3] = {{0.9f, 0.1f, 0.1f}, {0.3f, 0.9f, 0.3f}, {0.7f, 0.1f, 0.9f}};
            glColor4f(gc[i % 3][0], gc[i % 3][1], gc[i % 3][2], 1.0f);
        }
        glBegin(GL_QUADS);
        glVertex2f(gx2 - 2, gz2 - 2);
        glVertex2f(gx2 + 2, gz2 - 2);
        glVertex2f(gx2 + 2, gz2 + 2);
        glVertex2f(gx2 - 2, gz2 + 2);
        glEnd();
    }
}

void drawPowerBar(int w, int h)
{
    if (!powerActive)
        return;
    float ratio = (float)powerTimer / POWER_DURATION;
    float bx = w / 2 - 80;
    float by = h - 30;
    float bw = 160;
    float bh = 12;
    drawFilledRect(bx - 1, by - 1, bx + bw + 1, by + bh + 1, 0.5f, 0, 0.5f, 0.8f);
    float pulse = 0.7f + 0.3f * sinf(glutGet(GLUT_ELAPSED_TIME) / 80.0f);
    drawFilledRect(bx, by, bx + bw * ratio, by + bh, 0.7f * pulse, 0, 0.9f * pulse, 0.9f);
    glColor3f(1, 0.8f, 1);
    char buf[32];
    sprintf(buf, "POWER: %d", (int)(powerTimer / 60) + 1);
    renderStr(w / 2 - 30, by + 10, GLUT_BITMAP_HELVETICA_12, buf);
}

void drawJumpscare(int w, int h)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    if (jumpscareTexture)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, jumpscareTexture);
    }
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(0, 0);
    glTexCoord2f(1, 1);
    glVertex2f(w, 0);
    glTexCoord2f(1, 0);
    glVertex2f(w, h);
    glTexCoord2f(0, 0);
    glVertex2f(0, h);
    glEnd();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    if (jumpscareTexture)
        glDisable(GL_TEXTURE_2D);
}

void drawUI(int w, int h)
{
    if (showJumpscare)
        drawJumpscare(w, h);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    char buf[128];
    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    if (currentState == START)
    {
        drawFilledRect(w / 2 - 240, h / 2 - 140, w / 2 + 240, h / 2 + 200, 0, 0, 0, 0.88f);
        float c = 0.5f + 0.5f * sinf(t * 3);
        glColor3f(1.0f, 0.7f + 0.3f * c, 0.0f);
        renderStr(w / 2 - 85, h / 2 - 100, GLUT_BITMAP_TIMES_ROMAN_24, "KUCING GILA");
        glColor3f(0.6f, 0.6f + 0.3f * c, 1.0f);
        renderStr(w / 2 - 70, h / 2 - 70, GLUT_BITMAP_HELVETICA_12, "-- UPGRADED EDITION --");
        glColor3f(0.9f, 0.9f, 0.9f);
        renderStr(w / 2 - 130, h / 2 - 35, GLUT_BITMAP_HELVETICA_18, "Tekan ENTER untuk Memulai");
        glColor3f(0.75f, 0.75f, 0.75f);
        renderStr(w / 2 - 160, h / 2 + 10, GLUT_BITMAP_HELVETICA_12, "W/S = Maju/Mundur   A/D = Putar kiri/kanan");
        renderStr(w / 2 - 160, h / 2 + 30, GLUT_BITMAP_HELVETICA_12, "Mouse = Lihat kiri/kanan");
        renderStr(w / 2 - 160, h / 2 + 60, GLUT_BITMAP_HELVETICA_12, "Kumpulkan semua koin. Jangan ditangkap hantu!");
        glColor3f(0.8f, 0.2f, 0.9f);
        renderStr(w / 2 - 160, h / 2 + 90, GLUT_BITMAP_HELVETICA_12, "Koin UNGU = Power Pellet! Hantu jadi takut!");
        glColor3f(0.8f, 0.6f, 0.1f);
        renderStr(w / 2 - 160, h / 2 + 115, GLUT_BITMAP_HELVETICA_12, "Tangkap hantu saat power untuk SKOR COMBO!");
        if (highScore > 0)
        {
            glColor3f(1, 0.9f, 0);
            sprintf(buf, "High Score: %d", highScore);
            renderStr(w / 2 - 60, h / 2 + 150, GLUT_BITMAP_HELVETICA_18, buf);
        }
        glColor3f(0.6f, 0.1f, 0.1f);
        renderStr(w / 2 - 130, h / 2 + 175, GLUT_BITMAP_HELVETICA_12, "3 hantu akan mengejarmu di kegelapan...");
    }
    else if (currentState == PLAYING)
    {
        for (int i = 0; i < 3; i++)
        {
            glColor3f(i < lives ? 1.0f : 0.25f, i < lives ? 0.1f : 0.08f, 0.1f);
            drawHeart(28.f + i * 36.f, 28.f, 20.f);
        }
        glColor3f(0.8f, 0.5f, 1.0f);
        sprintf(buf, "Level %d", level);
        renderStr(w - 150, 135, GLUT_BITMAP_HELVETICA_12, buf);
        glColor3f(0.4f, 0.9f, 1.0f);
        sprintf(buf, "Koin: %d/%d", coins, totalCoins);
        renderStr(w - 150, 155, GLUT_BITMAP_HELVETICA_12, buf);
        glColor3f(1.f, 0.85f, 0.f);
        sprintf(buf, "Skor: %d", score);
        renderStr(w - 150, 175, GLUT_BITMAP_HELVETICA_18, buf);
        if (scoreMultiplier > 1)
        {
            float pulse = 0.7f + 0.3f * sinf(t * 8);
            glColor3f(1.0f, 0.5f * pulse, 0.0f);
            sprintf(buf, "x%d COMBO!", scoreMultiplier);
            renderStr(w / 2 - 40, h - 50, GLUT_BITMAP_TIMES_ROMAN_24, buf);
        }
        drawPowerBar(w, h);
        drawMinimap(w, h);
        if (shakeMag > 0.015f)
        {
            float alpha = 0.20f + 0.40f * fabsf(sinf(t * PI * 2));
            int bw2 = 7;
            glColor4f(0.9f, 0.0f, 0.0f, alpha);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(w, 0);
            glVertex2f(w, bw2);
            glVertex2f(0, bw2);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2f(0, h - bw2);
            glVertex2f(w, h - bw2);
            glVertex2f(w, h);
            glVertex2f(0, h);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(bw2, 0);
            glVertex2f(bw2, h);
            glVertex2f(0, h);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2f(w - bw2, 0);
            glVertex2f(w, 0);
            glVertex2f(w, h);
            glVertex2f(w - bw2, h);
            glEnd();
            glDisable(GL_BLEND);
        }
        if (powerActive)
        {
            float alpha = 0.15f + 0.15f * sinf(t * 6);
            int bw2 = 9;
            glColor4f(0.7f, 0.0f, 1.0f, alpha);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(w, 0);
            glVertex2f(w, bw2);
            glVertex2f(0, bw2);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2f(0, h - bw2);
            glVertex2f(w, h - bw2);
            glVertex2f(w, h);
            glVertex2f(0, h);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(bw2, 0);
            glVertex2f(bw2, h);
            glVertex2f(0, h);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2f(w - bw2, 0);
            glVertex2f(w, 0);
            glVertex2f(w, h);
            glVertex2f(w - bw2, h);
            glEnd();
            glDisable(GL_BLEND);
        }
    }
    else if (currentState == GAME_OVER)
    {
        drawFilledRect(w / 2 - 210, h / 2 - 120, w / 2 + 210, h / 2 + 120, 0, 0, 0, 0.88f);
        float pulse = 0.6f + 0.4f * sinf(t * 4);
        glColor3f(0.9f + 0.1f * pulse, 0.05f, 0.05f);
        renderStr(w / 2 - 75, h / 2 - 65, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
        glColor3f(0.8f, 0.8f, 0.8f);
        sprintf(buf, "Koin: %d / %d", coins, totalCoins);
        renderStr(w / 2 - 80, h / 2 - 20, GLUT_BITMAP_HELVETICA_18, buf);
        sprintf(buf, "Skor: %d", score);
        glColor3f(1, 0.85f, 0);
        renderStr(w / 2 - 45, h / 2 + 10, GLUT_BITMAP_HELVETICA_18, buf);
        if (score >= highScore && score > 0)
        {
            glColor3f(1, 0.9f, 0.2f);
            renderStr(w / 2 - 85, h / 2 + 38, GLUT_BITMAP_HELVETICA_12, "*** NEW HIGH SCORE! ***");
        }
        glColor3f(0.7f, 0.7f, 0.7f);
        renderStr(w / 2 - 110, h / 2 + 70, GLUT_BITMAP_HELVETICA_18, "Tekan  R  untuk Main Lagi");
    }
    else if (currentState == WIN)
    {
        drawFilledRect(w / 2 - 210, h / 2 - 120, w / 2 + 210, h / 2 + 120, 0, 0, 0, 0.88f);
        float r2 = 0.5f + 0.5f * sinf(t * 3);
        float g2 = 0.5f + 0.5f * sinf(t * 3 + 2);
        float b2 = 0.5f + 0.5f * sinf(t * 3 + 4);
        glColor3f(r2, g2, b2);
        renderStr(w / 2 - 60, h / 2 - 65, GLUT_BITMAP_TIMES_ROMAN_24, "YOU WIN!");
        glColor3f(0.8f, 0.8f, 0.8f);
        sprintf(buf, "Level %d Selesai!", level);
        renderStr(w / 2 - 80, h / 2 - 20, GLUT_BITMAP_HELVETICA_18, buf);
        sprintf(buf, "Skor: %d", score);
        glColor3f(1, 0.85f, 0);
        renderStr(w / 2 - 45, h / 2 + 10, GLUT_BITMAP_HELVETICA_18, buf);
        if (score >= highScore)
        {
            glColor3f(1, 0.9f, 0.2f);
            renderStr(w / 2 - 85, h / 2 + 38, GLUT_BITMAP_HELVETICA_12, "*** NEW HIGH SCORE! ***");
        }
        glColor3f(0.7f, 0.7f, 0.7f);
        renderStr(w / 2 - 115, h / 2 + 70, GLUT_BITMAP_HELVETICA_18, "Tekan  R  untuk Level Berikutnya");
    }
    if (flashA > 0.01f)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(flashR, flashG, flashB, flashA);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(w, 0);
        glVertex2f(w, h);
        glVertex2f(0, h);
        glEnd();
        glDisable(GL_BLEND);
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_FOG);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0)
        h = 1;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.f, (float)w / h, 0.05f, 140.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (currentState == START)
    {
        gluLookAt(0, 24, 10, 0, 0, 0, 0, 1, 0);
        drawMap();
        drawParticles();
    }
    else
    {
        updateCamera();
        updatePlayerLight();
        float rad = pAngle * PI / 180.f;
        float tgtX = pX + sinf(rad) * 4.f;
        float tgtY = 0.9f;
        float tgtZ = pZ + cosf(rad) * 4.f;
        gluLookAt(camX, camY, camZ, tgtX, tgtY, tgtZ, 0, 1, 0);
        drawMap();
        drawBlobShadow(pX, pZ, 0.35f);
        glPushMatrix();
        glTranslatef(pX, 0.38f, pZ);
        glRotatef(pAngle, 0, 1, 0);
        drawPacman(mouthAnim);
        glPopMatrix();
        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            if (!ghosts[i].active || ghosts[i].eaten)
                continue;
            drawBlobShadow(ghosts[i].x, ghosts[i].z, 0.28f);
            glPushMatrix();
            glTranslatef(ghosts[i].x, 0.5f, ghosts[i].z);
            float gAng = atan2f(pX - ghosts[i].x, pZ - ghosts[i].z) * 180.f / PI;
            glRotatef(gAng, 0, 1, 0);
            drawGhost(powerActive && ghosts[i].invincTimer == 0, 0.32f, i, (float)powerTimer);
            glPopMatrix();
        }
        drawParticles();
    }
    drawUI(w, h);
    glutSwapBuffers();
}

void updatePhysics(int value)
{
    updateFlash();
    updateParticles();
    if (showJumpscare)
    {
        jumpscareTimer--;
        if (jumpscareTimer <= 0)
        {
            showJumpscare = false;
            resetPlayer();
            for (int j = 0; j < NUM_GHOSTS; j++)
            {
                ghosts[j].active = false;
                ghosts[j].invincTimer = 120;
                spawnGhostRandom(j);
            }
        }
        glutPostRedisplay();
        glutTimerFunc(16, updatePhysics, 0);
        return;
    }
    if (currentState == PLAYING)
    {
        float spd = 0.10f;
        float turn = 3.5f;
        float rad = pAngle * PI / 180.f;
        float nx = pX;
        float nz = pZ;
        if (keys['w'] || arrowKeys[0])
        {
            nx += sinf(rad) * spd;
            nz += cosf(rad) * spd;
        }
        if (keys['s'] || arrowKeys[1])
        {
            nx -= sinf(rad) * spd;
            nz -= cosf(rad) * spd;
        }
        if (keys['a'] || arrowKeys[2])
            pAngle += turn;
        if (keys['d'] || arrowKeys[3])
            pAngle -= turn;
        if (!isWall(nx, pZ))
            pX = nx;
        if (!isWall(pX, nz))
            pZ = nz;
        bool moving = keys['w'] || keys['s'] || arrowKeys[0] || arrowKeys[1];
        if (moving)
        {
            mouthAnim += 4.f * mouthDir;
            if (mouthAnim > 35.f)
                mouthDir = -1.f;
            if (mouthAnim < 2.f)
                mouthDir = 1.f;
        }
        int itemGot = collectItem(pX, pZ);
        if (itemGot == 1)
        {
            coins++;
            score += 10 * scoreMultiplier;
            spawnParticles(pX, 0.5f, pZ, 0.3f, 0.8f, 1.0f, 8, 0.04f);
            feedbackBeep("coin");
        }
        else if (itemGot == 2)
        {
            coins++;
            score += 50 * scoreMultiplier;
            powerActive = true;
            powerTimer = POWER_DURATION;
            ghostEatCombo = 0;
            scoreMultiplier = 1;
            spawnParticles(pX, 0.5f, pZ, 0.8f, 0.0f, 1.0f, 30, 0.08f);
            triggerFlash(0.5f, 0, 0.8f, 0.5f);
            feedbackBeep("power");
        }
        if (powerActive)
        {
            powerTimer--;
            if (powerTimer <= 0)
            {
                powerActive = false;
                powerTimer = 0;
                ghostEatCombo = 0;
                scoreMultiplier = 1;
            }
        }
        if (multTimer > 0)
        {
            multTimer--;
            if (multTimer <= 0 && !powerActive)
                scoreMultiplier = 1;
        }
        float minDist = 1000.f;
        bool hitFrame = false;
        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            updateGhost(i);
            if (!ghosts[i].active || ghosts[i].eaten)
                continue;
            float ddx = pX - ghosts[i].x;
            float ddz = pZ - ghosts[i].z;
            float d = sqrtf(ddx * ddx + ddz * ddz);
            if (d < minDist)
                minDist = d;
            if (d < 0.75f && ghosts[i].invincTimer == 0 && !hitFrame)
            {
                hitFrame = true;
                if (powerActive)
                {
                    ghosts[i].eaten = true;
                    ghosts[i].respawnTimer = 200;
                    ghostEatCombo++;
                    int eatScore = 200 * (1 << (ghostEatCombo - 1));
                    score += eatScore * scoreMultiplier;
                    scoreMultiplier = ghostEatCombo + 1;
                    multTimer = 180;
                    spawnParticles(ghosts[i].x, 1.0f, ghosts[i].z, 1.0f, 0.8f, 0.0f, 40, 0.1f);
                    triggerFlash(1.0f, 0.8f, 0.0f, 0.4f);
                    feedbackBeep("eat");
                    shakeMag = 0.06f;
                }
                else
                {
                    if (lives > 1)
                    {
                        showJumpscare = true;
                        jumpscareTimer = 60;
                        lives--;
                        feedbackBeep("hit");
                        shakeMag = 0.28f;
                        triggerFlash(1.0f, 0.0f, 0.0f, 0.7f);
                    }
                    else
                    {
                        lives--;
                        if (lives <= 0)
                        {
                            if (score > highScore)
                                highScore = score;
                            currentState = GAME_OVER;
                            feedbackBeep("lose");
                            triggerFlash(1.0f, 0.0f, 0.0f, 0.8f);
                        }
                    }
                }
            }
        }
        updateFogColor(minDist);
        float shakeZone = 4.5f;
        float want = (minDist < shakeZone && !powerActive) ? ((shakeZone - minDist) / shakeZone * 0.18f) : 0.f;
        shakeMag = shakeMag * 0.80f + want * 0.20f;
        if (shakeMag < 0.001f)
            shakeMag = 0.f;
        if (coins >= totalCoins)
        {
            if (score > highScore)
                highScore = score;
            currentState = WIN;
            feedbackBeep("win");
            triggerFlash(1.0f, 0.95f, 0.0f, 0.8f);
            for (int k = 0; k < 5; k++)
            {
                spawnParticles((float)(rand() % 10 - 5), 1.0f, (float)(rand() % 10 - 5),
                               (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,
                               30, 0.12f);
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, updatePhysics, 0);
}

void mouseMotion(int mx, int my)
{
    if (mouseWarping)
    {
        mouseWarping = false;
        return;
    }
    if (currentState != PLAYING)
        return;
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    int cx = w / 2;
    int dx = mx - cx;
    if (dx != 0)
    {
        pAngle -= (float)dx * mouseSensitivity;
        mouseWarping = true;
        glutWarpPointer(cx, h / 2);
    }
}

void keyboard(unsigned char key, int x, int y)
{
    if (key >= 'A' && key <= 'Z')
        key += 32;
    keys[(int)key] = true;
    if (currentState == START && key == 13)
    {
        level = 1;
        initGame();
        currentState = PLAYING;
        glutSetCursor(GLUT_CURSOR_NONE);
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        mouseWarping = true;
        glutWarpPointer(w / 2, h / 2);
    }
    if (currentState == GAME_OVER && key == 'r')
    {
        level = 1;
        initGame();
        currentState = PLAYING;
        glutSetCursor(GLUT_CURSOR_NONE);
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        mouseWarping = true;
        glutWarpPointer(w / 2, h / 2);
    }
    if (currentState == WIN && key == 'r')
    {
        level++;
        int oldScore = score;
        initGame();
        score = oldScore;
        currentState = PLAYING;
        glutSetCursor(GLUT_CURSOR_NONE);
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        mouseWarping = true;
        glutWarpPointer(w / 2, h / 2);
    }
    if (key == 27)
    {
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        exit(0);
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (key >= 'A' && key <= 'Z')
        key += 32;
    keys[(int)key] = false;
}

void specialKey(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
        arrowKeys[0] = true;
    if (key == GLUT_KEY_DOWN)
        arrowKeys[1] = true;
    if (key == GLUT_KEY_LEFT)
        arrowKeys[2] = true;
    if (key == GLUT_KEY_RIGHT)
        arrowKeys[3] = true;
    if (currentState == START)
    {
        level = 1;
        initGame();
        currentState = PLAYING;
        glutSetCursor(GLUT_CURSOR_NONE);
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        mouseWarping = true;
        glutWarpPointer(w / 2, h / 2);
    }
}

void specialKeyUp(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
        arrowKeys[0] = false;
    if (key == GLUT_KEY_DOWN)
        arrowKeys[1] = false;
    if (key == GLUT_KEY_LEFT)
        arrowKeys[2] = false;
    if (key == GLUT_KEY_RIGHT)
        arrowKeys[3] = false;
}

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;
    glViewport(0, 0, w, h);
}
