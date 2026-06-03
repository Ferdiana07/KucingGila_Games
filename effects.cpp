#include "effects.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Particle particles[MAX_PARTICLES];
float flashR = 0, flashG = 0, flashB = 0, flashA = 0;

void spawnParticles(float x, float y, float z, float r, float g, float b, int count, float speed)
{
    int spawned = 0;

    for (int i = 0; i < MAX_PARTICLES && spawned < count; i++)
    {
        if (particles[i].active)
            continue;

        particles[i].active = true;
        particles[i].x = x;
        particles[i].y = y;
        particles[i].z = z;

        float ang = ((float)rand() / RAND_MAX) * 2 * PI;
        float elv = ((float)rand() / RAND_MAX) * PI - PI / 2;
        float spd = speed * (0.5f + (float)rand() / RAND_MAX);

        particles[i].vx = cosf(elv) * cosf(ang) * spd;
        particles[i].vy = fabsf(sinf(elv)) * spd + 0.03f;
        particles[i].vz = cosf(elv) * sinf(ang) * spd;

        particles[i].r = r;
        particles[i].g = g;
        particles[i].b = b;
        particles[i].a = 1.0f;
        particles[i].life = particles[i].maxLife = 30.0f + (float)(rand() % 30);
        particles[i].size = 0.04f + ((float)rand() / RAND_MAX) * 0.06f;
        spawned++;
    }
}

void updateParticles()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!particles[i].active)
            continue;

        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].z += particles[i].vz;
        particles[i].vy -= 0.003f;
        particles[i].life -= 1.0f;
        particles[i].a = particles[i].life / particles[i].maxLife;

        if (particles[i].life <= 0)
            particles[i].active = false;
    }
}

void drawParticles()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!particles[i].active)
            continue;

        glPushMatrix();
        glTranslatef(particles[i].x, particles[i].y, particles[i].z);
        glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].a);
        glutSolidSphere(particles[i].size, 4, 4);
        glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void triggerFlash(float r, float g, float b, float a)
{
    flashR = r;
    flashG = g;
    flashB = b;
    flashA = a;
}

void updateFlash()
{
    if (flashA > 0)
        flashA -= 0.04f;
    if (flashA < 0)
        flashA = 0;
}

void drawBlobShadow(float x, float z, float radius)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glColor4f(0, 0, 0, 0.45f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, 0.01f, z);
    for (int a = 0; a <= 24; a++)
    {
        float ang = a * (2 * PI / 24);
        glVertex3f(x + cosf(ang) * radius, 0.01f, z + sinf(ang) * radius);
    }
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

GLuint loadBMPTexture(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        printf("File BMP tidak ditemukan: %s\n", filename);
        return 0;
    }

    unsigned char header[54];
    fread(header, 1, 54, file);

    int width = *(int *)&header[18];
    int height = *(int *)&header[22];
    int imageSize = width * height * 3;
    unsigned char *data = (unsigned char *)malloc(imageSize);

    fread(data, 1, imageSize, file);
    fclose(file);

    for (int i = 0; i < imageSize; i += 3)
    {
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(data);

    return texture;
}
