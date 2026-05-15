#include "effects.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// ============== GLOBAL VARIABLES ==============
// Array untuk menyimpan max 400 partikel efek visual
Particle particles[MAX_PARTICLES];

// Color flash effect - digunakan untuk kilatan layar saat ada event penting
float flashR = 0, flashG = 0, flashB = 0, flashA = 0;

// ============== SPAWN PARTICLES ==============
// Fungsi: Spawn partikel baru untuk efek visual (ledakan, dll)
// Input: x,y,z = posisi spawn, r,g,b = warna RGB, count = jumlah partikel, speed = kecepatan awal
void spawnParticles(float x, float y, float z, float r, float g, float b, int count, float speed)
{
    int spawned = 0;

    // Loop cari slot partikel kosong dan spawn
    for (int i = 0; i < MAX_PARTICLES && spawned < count; i++)
    {
        // Skip jika slot sudah terpakai
        if (particles[i].active)
            continue;

        // Aktifkan partikel dan set posisi awal
        particles[i].active = true;
        particles[i].x = x;
        particles[i].y = y;
        particles[i].z = z;

        // Generate random direction untuk partikel (spherical coordinates)
        float ang = ((float)rand() / RAND_MAX) * 2 * PI;      // Azimuth angle random
        float elv = ((float)rand() / RAND_MAX) * PI - PI / 2; // Elevation angle random

        // Randomize kecepatan sedikit (50-150% dari input speed)
        float spd = speed * (0.5f + (float)rand() / RAND_MAX);

        // Convert spherical ke cartesian untuk velocity
        particles[i].vx = cosf(elv) * cosf(ang) * spd;
        particles[i].vy = fabsf(sinf(elv)) * spd + 0.03f; // +0.03f untuk upward bias
        particles[i].vz = cosf(elv) * sinf(ang) * spd;

        // Set warna partikel
        particles[i].r = r;
        particles[i].g = g;
        particles[i].b = b;
        particles[i].a = 1.0f; // Fully opaque awalnya

        // Set lifetime partikel (30-60 frames)
        particles[i].life = particles[i].maxLife = 30.0f + (float)(rand() % 30);

        // Set ukuran partikel (random 0.04-0.10 radius)
        particles[i].size = 0.04f + ((float)rand() / RAND_MAX) * 0.06f;

        spawned++;
    }
}

// ============== UPDATE PARTICLES ==============
// Fungsi: Update semua partikel setiap frame (physics dan fade-out)
void updateParticles()
{
    // Loop semua slot partikel
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        // Skip slot partikel yang tidak aktif
        if (!particles[i].active)
            continue;

        // Update posisi dengan menambahkan velocity
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].z += particles[i].vz;

        // Apply gravity: reduce upward velocity (simulate gravity effect)
        particles[i].vy -= 0.003f;

        // Decrement lifetime counter
        particles[i].life -= 1.0f;

        // Update alpha (transparency) - semakin lama semakin transparan
        particles[i].a = particles[i].life / particles[i].maxLife;

        // Deaktifkan partikel jika sudah mati
        if (particles[i].life <= 0)
            particles[i].active = false;
    }
}

// ============== DRAW PARTICLES ==============
// Fungsi: Render semua partikel sebagai glowing spheres dengan blending
void drawParticles()
{
    // Disable lighting karena partikel harus bersinar/glow
    glDisable(GL_LIGHTING);

    // Enable blending untuk transparency effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending untuk glow effect

    // Disable depth write agar partikel tidak menutupi depth
    glDepthMask(GL_FALSE);

    // Loop dan render setiap partikel aktif
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!particles[i].active)
            continue;

        // Push matrix untuk posisi partikel
        glPushMatrix();
        glTranslatef(particles[i].x, particles[i].y, particles[i].z);

        // Set warna dengan alpha transparency yang gradual fade-out
        glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].a);

        // Draw sphere sebagai partikel (4x4 resolution untuk performa)
        glutSolidSphere(particles[i].size, 4, 4);

        glPopMatrix();
    }

    // Re-enable depth mask dan restore lighting
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ============== TRIGGER FLASH EFFECT ==============
// Fungsi: Trigger kilatan layar dengan warna RGB dan durasi (alpha)
// Digunakan saat ada event penting: makan koin, makan hantu, mati, dll
void triggerFlash(float r, float g, float b, float a)
{
    flashR = r;
    flashG = g;
    flashB = b;
    flashA = a; // Set durasi flash
}

// ============== UPDATE FLASH EFFECT ==============
// Fungsi: Fade-out flash effect setiap frame (decrement alpha)
void updateFlash()
{
    // Decrement alpha untuk fade-out effect
    if (flashA > 0)
        flashA -= 0.04f; // Fade-out rate = 0.04 per frame

    // Clamp alpha ke 0 agar tidak negative
    if (flashA < 0)
        flashA = 0;
}

// ============== DRAW BLOB SHADOW ==============
// Fungsi: Draw shadow blob (bayangan) di bawah karakter
// Digunakan untuk shadow karakter pacman dan hantu agar terlihat terhubung dengan ground
void drawBlobShadow(float x, float z, float radius)
{
    // Disable lighting agar shadow consistent
    glDisable(GL_LIGHTING);

    // Enable blending untuk semi-transparent shadow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth write agar shadow tidak menutupi ground
    glDepthMask(GL_FALSE);

    // Set shadow color (black, semi-transparent)
    glColor4f(0, 0, 0, 0.45f);

    // Draw circular shadow sebagai triangle fan
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, 0.01f, z); // Center point sedikit di atas ground (0.01f)

    // Draw circle perimeter (24 segments untuk smooth circle)
    for (int a = 0; a <= 24; a++)
    {
        float ang = a * (2 * PI / 24); // 360 derajat / 24 segments
        glVertex3f(x + cosf(ang) * radius, 0.01f, z + sinf(ang) * radius);
    }
    glEnd();

    // Restore OpenGL state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ============== LOAD BMP TEXTURE ==============
// Fungsi: Load file BMP dan convert ke OpenGL texture format
// Return: texture ID untuk digunakan dengan glBindTexture
GLuint loadBMPTexture(const char *filename)
{
    // Open BMP file dalam binary read mode
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        printf("File BMP tidak ditemukan: %s\n", filename);
        return 0; // Return 0 jika file tidak ada
    }

    // Read BMP header (54 bytes standar untuk BMP file)
    unsigned char header[54];
    fread(header, 1, 54, file);

    // Extract width dan height dari header (offset 18 dan 22)
    int width = *(int *)&header[18];
    int height = *(int *)&header[22];

    // Calculate image data size (24-bit RGB = 3 bytes per pixel)
    int imageSize = width * height * 3;

    // Allocate memory untuk image data
    unsigned char *data = (unsigned char *)malloc(imageSize);

    // Read pixel data dari file
    fread(data, 1, imageSize, file);
    fclose(file);

    // BMP format store pixels sebagai BGR (not RGB), jadi perlu swap R dan B channels
    for (int i = 0; i < imageSize; i += 3)
    {
        unsigned char tmp = data[i]; // Temp store B channel
        data[i] = data[i + 2];       // Move R ke position B
        data[i + 2] = tmp;           // Move B ke position R
    }

    // Create OpenGL texture
    GLuint texture;
    glGenTextures(1, &texture);            // Generate texture ID
    glBindTexture(GL_TEXTURE_2D, texture); // Make this texture active

    // Upload pixel data ke GPU sebagai texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // Set texture filtering mode (LINEAR untuk smooth texture saat di-scale)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filter saat zoom out
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filter saat zoom in

    // Free memory yang sudah di-upload ke GPU
    free(data);

    // Return texture ID untuk digunakan di glBindTexture nanti
    return texture;
}
