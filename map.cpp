#include "map.h"
#include "effects.h"
#include <math.h>

// 0 = jalan, 1 = tembok, 2 = koin, 3 = power pellet
int initial_maze[MAP_ROWS][MAP_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 2, 0, 2, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 2, 0, 1, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 1, 2, 0, 1},
    {1, 2, 0, 1, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 2, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 2, 0, 1, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 1, 2, 0, 1},
    {1, 3, 0, 2, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 0, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 0, 3},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 2, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 2, 1, 1, 1, 0, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 2, 1, 1, 1},
    {0, 0, 1, 2, 2, 1, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0, 0, 0, 0, 2, 0, 0, 1, 2, 2, 1, 0, 1, 2, 2, 1, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 2, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 1, 0, 0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 1, 0, 0},
    {0, 0, 1, 2, 2, 1, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0, 1, 0, 0, 2, 0, 0, 1, 2, 2, 1, 0, 1, 2, 2, 1, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0},
    {1, 1, 1, 2, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 2, 1, 1, 1, 0, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 2, 1, 1, 1},
    {1, 2, 0, 2, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 0, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 2, 0, 1, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 1, 2, 0, 1},
    {1, 2, 0, 1, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 0, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 1, 2, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 2, 0, 1, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 1, 2, 0, 1},
    {1, 3, 0, 2, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 0, 3},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int maze[MAP_ROWS][MAP_COLS];
const float offsetX = (MAP_COLS - 1) / 2.0f;
const float offsetZ = (MAP_ROWS - 1) / 2.0f;

static void setWallMaterial(int row, int col)
{
    GLfloat amb[4], diff[4], spec[4], emi[4] = {0, 0, 0, 1};
    float pulse = 0.5f + 0.5f * sinf(glutGet(GLUT_ELAPSED_TIME) / 850.0f + row * 0.27f + col * 0.33f);

    amb[0] = 0.015f;
    amb[1] = 0.055f;
    amb[2] = 0.070f;
    amb[3] = 1;

    diff[0] = 0.035f;
    diff[1] = 0.120f;
    diff[2] = 0.155f;
    diff[3] = 1;

    spec[0] = 0.25f;
    spec[1] = 0.75f;
    spec[2] = 0.95f;
    spec[3] = 1;

    emi[0] = 0.000f;
    emi[1] = 0.055f + 0.045f * pulse;
    emi[2] = 0.110f + 0.070f * pulse;
    emi[3] = 1;

    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT, GL_EMISSION, emi);
    glMaterialf(GL_FRONT, GL_SHININESS, 80.0f);
}

static void drawFloorDisc(float cx, float zc, float radius, float y)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(cx, y, zc);
    for (int a = 0; a <= 14; a++)
    {
        float ang = a * (2 * PI / 14);
        glVertex3f(cx + cosf(ang) * radius, y, zc + sinf(ang) * radius);
    }
    glEnd();
}

static void drawFloorTextureOverlay(float x, float z, int row, int col)
{
    float y = 0.018f;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(1.0f);
    glColor4f(0.08f, 0.42f, 0.55f, 0.22f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x - 0.48f, y, z - 0.48f);
    glVertex3f(x - 0.48f, y, z + 0.48f);
    glVertex3f(x + 0.48f, y, z + 0.48f);
    glVertex3f(x + 0.48f, y, z - 0.48f);
    glEnd();

    if ((row * 3 + col * 5) % 11 == 0)
    {
        glColor4f(0.70f, 0.95f, 1.0f, 0.34f);
        drawFloorDisc(x, z, 0.085f, y + 0.004f);
        drawFloorDisc(x - 0.09f, z + 0.10f, 0.035f, y + 0.004f);
        drawFloorDisc(x, z + 0.13f, 0.035f, y + 0.004f);
        drawFloorDisc(x + 0.09f, z + 0.10f, 0.035f, y + 0.004f);
    }
    else if ((row * 7 + col * 2) % 13 == 0)
    {
        glColor4f(0.65f, 0.90f, 1.0f, 0.32f);
        glBegin(GL_LINES);
        glVertex3f(x - 0.18f, y + 0.004f, z);
        glVertex3f(x + 0.18f, y + 0.004f, z);
        for (float s = -0.10f; s <= 0.10f; s += 0.10f)
        {
            glVertex3f(x + s, y + 0.004f, z);
            glVertex3f(x + s - 0.07f, y + 0.004f, z + 0.08f);
            glVertex3f(x + s, y + 0.004f, z);
            glVertex3f(x + s + 0.07f, y + 0.004f, z - 0.08f);
        }
        glEnd();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void drawPillar(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0, z);
    float pulse = 0.5f + 0.5f * sinf(glutGet(GLUT_ELAPSED_TIME) / 600.0f + x * 0.4f + z * 0.7f);
    GLfloat pAmb[] = {0.05f, 0.05f, 0.1f, 1};
    GLfloat pDiff[] = {0.1f, 0.1f, 0.25f, 1};
    GLfloat pSpec[] = {0.6f, 0.6f, 1.0f, 1};
    GLfloat pEmi[] = {0.05f * pulse, 0.05f * pulse, 0.2f * pulse, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, pAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pDiff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, pSpec);
    glMaterialfv(GL_FRONT, GL_EMISSION, pEmi);
    glMaterialf(GL_FRONT, GL_SHININESS, 90);
    GLUquadric *q = gluNewQuadric();
    gluCylinder(q, 0.12f, 0.10f, 3.0f, 10, 4);
    gluDeleteQuadric(q);
    glTranslatef(0, 3.0f, 0);
    glutSolidSphere(0.13f, 8, 8);
    glPopMatrix();
}

static void drawWallTextureOverlay(float x, float z, float wallH, int row, int col)
{
    float pulse = 0.35f + 0.65f * sinf(glutGet(GLUT_ELAPSED_TIME) / 700.0f + row * 0.4f + col * 0.25f);
    float brickShift = ((row + col) % 2) ? 0.25f : 0.0f;
    float edge = 0.506f;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0f);

    glColor4f(0.0f, 0.0f, 0.0f, 0.34f);
    for (int face = 0; face < 4; face++)
    {
        for (float y = 0.55f; y < wallH; y += 0.55f)
        {
            glBegin(GL_LINES);
            if (face == 0)
            {
                glVertex3f(x - edge, y, z - edge);
                glVertex3f(x + edge, y, z - edge);
            }
            else if (face == 1)
            {
                glVertex3f(x - edge, y, z + edge);
                glVertex3f(x + edge, y, z + edge);
            }
            else if (face == 2)
            {
                glVertex3f(x - edge, y, z - edge);
                glVertex3f(x - edge, y, z + edge);
            }
            else
            {
                glVertex3f(x + edge, y, z - edge);
                glVertex3f(x + edge, y, z + edge);
            }
            glEnd();
        }

        for (float y = 0.0f; y < wallH; y += 1.10f)
        {
            for (float s = -0.25f + brickShift; s < 0.51f; s += 0.50f)
            {
                float y2 = fminf(y + 0.55f, wallH);
                glBegin(GL_LINES);
                if (face == 0)
                {
                    glVertex3f(x + s, y, z - edge);
                    glVertex3f(x + s, y2, z - edge);
                }
                else if (face == 1)
                {
                    glVertex3f(x + s, y, z + edge);
                    glVertex3f(x + s, y2, z + edge);
                }
                else if (face == 2)
                {
                    glVertex3f(x - edge, y, z + s);
                    glVertex3f(x - edge, y2, z + s);
                }
                else
                {
                    glVertex3f(x + edge, y, z + s);
                    glVertex3f(x + edge, y2, z + s);
                }
                glEnd();
            }
        }
    }

    if ((row * 7 + col * 11) % 5 == 0)
    {
        glColor4f(0.2f + 0.35f * pulse, 0.75f * pulse, 1.0f * pulse, 0.45f);
        glBegin(GL_LINE_STRIP);
        glVertex3f(x - 0.30f, wallH * 0.82f, z - edge - 0.002f);
        glVertex3f(x - 0.10f, wallH * 0.62f, z - edge - 0.002f);
        glVertex3f(x - 0.18f, wallH * 0.42f, z - edge - 0.002f);
        glVertex3f(x + 0.18f, wallH * 0.24f, z - edge - 0.002f);
        glEnd();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void drawCeilingTextureOverlay(float x, float z, float y, int row, int col)
{
    if ((row * 5 + col * 3) % 9 != 0)
        return;

    float pulse = 0.55f + 0.45f * sinf(glutGet(GLUT_ELAPSED_TIME) / 500.0f + row + col);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glLineWidth(1.0f);
    glColor4f(0.25f, 0.85f, 1.0f, 0.18f + 0.20f * pulse);
    glBegin(GL_LINES);
    glVertex3f(x - 0.13f, y - 0.018f, z);
    glVertex3f(x + 0.13f, y - 0.018f, z);
    glVertex3f(x, y - 0.018f, z - 0.13f);
    glVertex3f(x, y - 0.018f, z + 0.13f);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void drawCeiling()
{
    float wallH = 3.2f;
    GLfloat ca[] = {0.01f, 0.01f, 0.02f, 1};
    GLfloat cd[] = {0.02f, 0.02f, 0.04f, 1};
    GLfloat cs[] = {0.05f, 0.05f, 0.1f, 1};
    GLfloat noE[] = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, ca);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, cs);
    glMaterialfv(GL_FRONT, GL_EMISSION, noE);
    glMaterialf(GL_FRONT, GL_SHININESS, 5);

    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (maze[i][j] == 1)
                continue;
            float x = j - offsetX;
            float z = i - offsetZ;
            glBegin(GL_QUADS);
            glNormal3f(0, -1, 0);
            glVertex3f(x - 0.5f, wallH, z - 0.5f);
            glVertex3f(x + 0.5f, wallH, z - 0.5f);
            glVertex3f(x + 0.5f, wallH, z + 0.5f);
            glVertex3f(x - 0.5f, wallH, z + 0.5f);
            glEnd();
            drawCeilingTextureOverlay(x, z, wallH, i, j);
        }
    }
}

void resetMap()
{
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            maze[i][j] = initial_maze[i][j];
        }
    }
}

int getTotalCoins()
{
    int n = 0;
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (maze[i][j] == 2 || maze[i][j] == 3)
                n++;
        }
    }
    return n;
}

bool isFreeCell(int row, int col)
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
        return false;
    return maze[row][col] != 1;
}

bool isWall(float x, float z)
{
    const float pad = 0.42f;
    float pts[4][2] = {{x + pad, z + pad}, {x + pad, z - pad}, {x - pad, z + pad}, {x - pad, z - pad}};
    for (int k = 0; k < 4; k++)
    {
        int col = (int)roundf(pts[k][0] + offsetX);
        int row = (int)roundf(pts[k][1] + offsetZ);
        if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
            return true;
        if (maze[row][col] == 1)
            return true;
    }
    return false;
}

bool isWallCamera(float x, float z)
{
    int col = (int)roundf(x + offsetX);
    int row = (int)roundf(z + offsetZ);
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
        return true;
    return maze[row][col] == 1;
}

int collectItem(float x, float z)
{
    int col = (int)roundf(x + offsetX);
    int row = (int)roundf(z + offsetZ);
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
    {
        if (maze[row][col] == 2)
        {
            maze[row][col] = 0;
            return 1;
        }
        if (maze[row][col] == 3)
        {
            maze[row][col] = 0;
            return 2;
        }
    }
    return 0;
}

void drawMap()
{
    float wallH = 3.2f;
    float halfH = wallH * 0.5f;
    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (maze[i][j] == 1)
                continue;
            float x = j - offsetX;
            float z = i - offsetZ;
            bool dark = (i + j) % 2 == 0;
            GLfloat fa[4], fd[4], fspec[4], noE[4] = {0, 0, 0, 1};
            if (dark)
            {
                fa[0] = 0.01f;
                fa[1] = 0.01f;
                fa[2] = 0.02f;
                fa[3] = 1;
                fd[0] = 0.025f;
                fd[1] = 0.02f;
                fd[2] = 0.05f;
                fd[3] = 1;
                fspec[0] = 0.15f;
                fspec[1] = 0.15f;
                fspec[2] = 0.3f;
                fspec[3] = 1;
            }
            else
            {
                fa[0] = 0.02f;
                fa[1] = 0.02f;
                fa[2] = 0.03f;
                fa[3] = 1;
                fd[0] = 0.05f;
                fd[1] = 0.04f;
                fd[2] = 0.07f;
                fd[3] = 1;
                fspec[0] = 0.25f;
                fspec[1] = 0.25f;
                fspec[2] = 0.4f;
                fspec[3] = 1;
            }
            glMaterialfv(GL_FRONT, GL_AMBIENT, fa);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, fd);
            glMaterialfv(GL_FRONT, GL_SPECULAR, fspec);
            glMaterialf(GL_FRONT, GL_SHININESS, 20);
            glMaterialfv(GL_FRONT, GL_EMISSION, noE);
            glBegin(GL_QUADS);
            glNormal3f(0, 1, 0);
            glVertex3f(x - 0.5f, 0, z - 0.5f);
            glVertex3f(x - 0.5f, 0, z + 0.5f);
            glVertex3f(x + 0.5f, 0, z + 0.5f);
            glVertex3f(x + 0.5f, 0, z - 0.5f);
            glEnd();
            drawFloorTextureOverlay(x, z, i, j);
        }
    }

    drawCeiling();
    GLfloat noEmi[] = {0, 0, 0, 1};

    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            float x = j - offsetX;
            float z = i - offsetZ;

            if (maze[i][j] == 1)
            {
                setWallMaterial(i, j);
                glPushMatrix();
                glTranslatef(x, halfH, z);
                glScalef(1.0f, wallH, 1.0f);
                glutSolidCube(1.0f);
                glPopMatrix();
                drawWallTextureOverlay(x, z, wallH, i, j);
                if (i > 0 && i < MAP_ROWS - 1 && j > 0 && j < MAP_COLS - 1)
                {
                    bool a = maze[i - 1][j] == 1, b = maze[i + 1][j] == 1;
                    bool c = maze[i][j - 1] == 1, d = maze[i][j + 1] == 1;
                    if (!a && !b && !c && !d && (i + j) % 8 == 0)
                    {
                        drawPillar(x, z);
                    }
                }
            }
            else if (maze[i][j] == 2)
            {
                GLfloat ca[] = {0.1f, 0.4f, 0.5f, 1};
                GLfloat cd[] = {0.4f, 0.8f, 1.0f, 1};
                GLfloat cs[] = {1, 1, 1, 1};
                GLfloat ce[] = {0.0f, 0.6f, 0.8f, 1};
                glMaterialfv(GL_FRONT, GL_AMBIENT, ca);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, cd);
                glMaterialfv(GL_FRONT, GL_SPECULAR, cs);
                glMaterialfv(GL_FRONT, GL_EMISSION, ce);
                glMaterialf(GL_FRONT, GL_SHININESS, 100);
                float fishY = 0.38f + 0.07f * sinf(x + z + t * 5.5f);
                float fishRot = t * 45.0f + (x * 37.f + z * 53.f);
                glPushMatrix();
                glTranslatef(x, fishY, z);
                glRotatef(fishRot, 0, 1, 0);
                glPushMatrix();
                glScalef(1.0f, 0.5f, 0.6f);
                glutSolidSphere(0.15f, 10, 10);
                glPopMatrix();
                glPushMatrix();
                glTranslatef(-0.15f, 0, 0);
                glRotatef(90, 0, 1, 0);
                glutSolidCone(0.09f, 0.11f, 7, 3);
                glPopMatrix();
                glMaterialfv(GL_FRONT, GL_EMISSION, noEmi);
                glPopMatrix();
                drawBlobShadow(x, z, 0.12f);
            }
            else if (maze[i][j] == 3)
            {
                float pulse2 = 0.7f + 0.3f * sinf(t * 6.0f + x + z);
                GLfloat ppa[] = {0.3f, 0.0f, 0.3f, 1};
                GLfloat ppd[] = {0.8f, 0.0f, 0.8f, 1};
                GLfloat pps[] = {1, 0.5f, 1, 1};
                GLfloat ppe[] = {0.6f * pulse2, 0.0f, 0.9f * pulse2, 1};
                glMaterialfv(GL_FRONT, GL_AMBIENT, ppa);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, ppd);
                glMaterialfv(GL_FRONT, GL_SPECULAR, pps);
                glMaterialfv(GL_FRONT, GL_EMISSION, ppe);
                glMaterialf(GL_FRONT, GL_SHININESS, 120);
                float ppY = 0.38f + 0.1f * sinf(t * 4.0f + x * 2 + z);
                glPushMatrix();
                glTranslatef(x, ppY, z);
                glRotatef(t * 90.0f, 0, 1, 0);
                glutSolidSphere(0.22f * pulse2, 14, 14);
                glPopMatrix();
                glPushMatrix();
                glTranslatef(x, ppY, z);
                glRotatef(t * -60.0f, 1, 1, 0);
                GLfloat re[] = {0.4f * pulse2, 0, 0.6f * pulse2, 1};
                glMaterialfv(GL_FRONT, GL_EMISSION, re);
                glutWireSphere(0.28f, 10, 10);
                glPopMatrix();
                drawBlobShadow(x, z, 0.2f);
                glMaterialfv(GL_FRONT, GL_EMISSION, noEmi);
            }
        }
    }
}
