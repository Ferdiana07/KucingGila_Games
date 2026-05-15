#include "ghost.h"
#include "map.h"
#include "effects.h"
#include <math.h>

// ============== GLOBAL VARIABLE ==============
// Array menyimpan data 3 hantu (posisi, kecepatan, state, dll)
Ghost ghosts[NUM_GHOSTS];

// ============== SPAWN GHOST RANDOM ==============
// Fungsi: Spawn hantu ke-idx di posisi random yang jauh dari pemain
void spawnGhostRandom(int idx)
{
    // Try spawn hingga 600 kali untuk cari posisi yang bagus
    for (int attempt = 0; attempt < 600; attempt++)
    {
        // Generate random grid position (row, col)
        int row = rand() % MAP_ROWS;
        int col = rand() % MAP_COLS;

        // Skip jika cell adalah tembok (bukan free cell)
        if (!isFreeCell(row, col))
            continue;

        // Convert grid coords ke world coords
        // offsetX/Z = center peta (16, 12 untuk peta 33x25)
        float wx = col - (MAP_COLS - 1) * 0.5f;
        float wz = row - (MAP_ROWS - 1) * 0.5f;

        // Calculate distance ke pemain
        float dx = wx - pX;
        float dz = wz - pZ;

        // Jika hantu spawn minimum 6 unit jauh dari pemain
        if (sqrtf(dx * dx + dz * dz) > 6.f)
        {
            // Set hantu posisi dan aktifkan
            ghosts[idx].x = wx;
            ghosts[idx].z = wz;
            ghosts[idx].active = true;
            ghosts[idx].eaten = false;
            return; // Berhasil spawn, keluar
        }
    }

    // Fallback: jika tidak ketemu posisi bagus, spawn di hardcoded position
    // Fallback posisi tersebar: idx*4-4 = -4, 0, 4 untuk 3 hantu
    ghosts[idx].x = (float)(idx * 4 - 4);
    ghosts[idx].z = -5.f;
    ghosts[idx].active = true;
    ghosts[idx].eaten = false;
}

// ============== UPDATE GHOST AI ==============
// Fungsi: Update AI logic hantu ke-i setiap frame
void updateGhost(int i)
{
    Ghost &g = ghosts[i];

    // Skip jika hantu tidak aktif
    if (!g.active)
        return;

    // ============== STATE: HANTU DIMAKAN ==============
    if (g.eaten)
    {
        // Decrement respawn timer (berapa lama hantu di-respawn)
        g.respawnTimer--;

        // Jika timer selesai, respawn hantu
        if (g.respawnTimer <= 0)
        {
            // Reset eaten flag
            g.eaten = false;

            // Set invincibility timer (120 frame = 2 detik) - tidak bisa dimakan jadi
            g.invincTimer = 120;

            // Spawn hantu di random posisi baru
            spawnGhostRandom(i);

            // Spawn partikel respawn dengan warna sesuai type hantu
            // Type 0 (red) = 0.9,0.1,0.1 | Type 1 (blue) = 0.3,0.9,0.3 | Type 2 (pink) = 0.6,0.1,0.9
            spawnParticles(g.x, 1.0f, g.z,
                           g.type == 0 ? 0.9f : (g.type == 1 ? 0.3f : 0.6f),
                           g.type == 0 ? 0.1f : (g.type == 1 ? 0.9f : 0.1f),
                           g.type == 0 ? 0.1f : (g.type == 1 ? 0.3f : 0.9f),
                           20, 0.05f);
        }
        return; // Keluar dari update, hantu masih dimakan
    }

    // ============== STATE: INVINCIBILITY ==============
    // Saat baru respawn, hantu kebal selama 120 frame (tidak bisa dimakan)
    if (g.invincTimer > 0)
    {
        g.invincTimer--;
        return; // Hantu kebal, skip AI logic
    }

    // ============== CALCULATE DIRECTION TO PLAYER ==============
    // Hitung vektor dari hantu ke pemain
    float dx = pX - g.x;
    float dz = pZ - g.z;

    // Hitung jarak (distance)
    float dist = sqrtf(dx * dx + dz * dz);

    // Normalize direction vector (nx, nz)
    float nx = 0, nz = 0;
    if (dist > 0.01f) // Avoid division by zero
    {
        nx = dx / dist;
        nz = dz / dist;
    }

    // ============== POWER MODE - HANTU TAKUT DAN LARI ==============
    if (powerActive)
    {
        // Reverse direction: hantu lari dari pemain
        nx = -nx;
        nz = -nz;
    }

    // ============== SMART CHASE - TYPE 1 (BLUE GHOST) ==============
    // Type 1 hantu cerdas: ambil posisi depan pemain, bukan chase langsung
    if (g.type == 1 && !powerActive && dist > 1.5f)
    {
        // Hitung posisi depan pemain (3 unit di depan arah pemain)
        float playerRad = pAngle * PI / 180.f;  // Convert angle to radian
        float tx = pX + sinf(playerRad) * 3.0f; // Predict target X
        float tz = pZ + cosf(playerRad) * 3.0f; // Predict target Z

        // Hitung vektor dari hantu ke target
        float fdx = tx - g.x;
        float fdz = tz - g.z;
        float fd = sqrtf(fdx * fdx + fdz * fdz);

        // Normalize direction ke target
        if (fd > 0.01f)
        {
            nx = fdx / fd;
            nz = fdz / fd;
        }
    }

    // ============== APPLY SPEED & MOVEMENT ==============
    // Saat power mode: hantu speed 60% lebih lambat (0.6x speed)
    float spd = g.speed * (powerActive ? 0.6f : (nightmareActive ? 1.42f : 1.0f));

    // Calculate proposed new position
    float gx2 = g.x + nx * spd;
    float gz2 = g.z + nz * spd;

    // Check collision dengan dinding untuk X dan Z direction secara terpisah
    bool hx = isWall(gx2, g.z); // Check collision untuk X movement
    bool hz = isWall(g.x, gz2); // Check collision untuk Z movement

    // Apply movement jika tidak ada collision
    if (!hx)
        g.x = gx2;
    if (!hz)
        g.z = gz2;

    // ============== STUCK DETECTION & WANDERING ==============
    // Jika hantu terperangkap di corner (collision di kedua axis)
    if (hx && hz)
    {
        // Increment stuck timer
        g.stuckTimer++;

        // Jika stuck terlalu lama (>8 frame), random direction baru
        if (g.stuckTimer > 8)
        {
            // Add random angle 90, 180, 270 derajat
            g.wanderAngle += 90.f * (float)(1 + rand() % 3);
            g.stuckTimer = 0;
        }

        // Coba movement dengan wander angle
        float wr = g.wanderAngle * PI / 180.f;
        float wx2 = g.x + sinf(wr) * spd;
        float wz2 = g.z + cosf(wr) * spd;

        // Apply wander movement jika tidak collision
        if (!isWall(wx2, g.z))
            g.x = wx2;
        if (!isWall(g.x, wz2))
            g.z = wz2;
    }
    else
    {
        // Reset stuck timer jika hantu berhasil move
        g.stuckTimer = 0;
    }
}

// ============== DRAW GHOST ==============
// Fungsi: Render 3D model hantu dengan material dan animasi
// frightened = true saat power mode (hantu biru takut)
// scale = ukuran hantu (1.0 = normal)
// ghostIndex = index hantu (0-2) untuk tentukan warna
// frightenTimer = sisa durasi power mode (untuk blinking effect)
void drawGhost(bool frightened, float scale, int ghostIndex, float frightenTimer)
{
    // ============== SETUP TRANSFORMATION ==============
    glPushMatrix();
    glScalef(scale, scale, scale);

    // ============== SETUP MATERIAL COLORS ==============
    // Warna hantu dibuat lebih gelap agar terlihat lebih seram di labirin.
    GLfloat bodyColors[3][3] = {
        {0.48f, 0.02f, 0.02f}, // Type 0: dark red
        {0.08f, 0.34f, 0.12f}, // Type 1: dark green
        {0.24f, 0.00f, 0.36f}, // Type 2: dark purple
    };

    // Get current ghost color index (modulo 3 untuk safety)
    int idx = ghostIndex % 3;

    // Get current time untuk animasi
    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    // ============== SET COLOR BASED ON STATE ==============
    GLfloat body_diff[4], body_amb[4];

    if (frightened)
    {
        // FRIGHTENED (POWER MODE): Warna biru dengan blinking effect
        // Blinking: setiap 100ms (~10 frame), blink off untuk 90ms last frame
        float blink = (frightenTimer < 90 && (int)(frightenTimer / 10) % 2 == 0) ? 0.8f : 1.0f;

        // Diffuse: biru dengan blinking intensity
        body_diff[0] = 0.05f * blink;
        body_diff[1] = 0.05f * blink;
        body_diff[2] = 0.9f * blink;
        body_diff[3] = 1;

        // Ambient: dark blue
        body_amb[0] = 0.02f;
        body_amb[1] = 0.02f;
        body_amb[2] = 0.3f;
        body_amb[3] = 1;
    }
    else
    {
        // NORMAL MODE: Warna sesuai type hantu
        body_diff[0] = bodyColors[idx][0];
        body_diff[1] = bodyColors[idx][1];
        body_diff[2] = bodyColors[idx][2];
        body_diff[3] = 1;

        // Ambient lebih gelap agar bentuknya terasa horror saat disorot.
        body_amb[0] = bodyColors[idx][0] * 0.18f;
        body_amb[1] = bodyColors[idx][1] * 0.18f;
        body_amb[2] = bodyColors[idx][2] * 0.18f;
        body_amb[3] = 1;
    }

    // ============== SETUP MATERIAL PROPERTIES ==============
    GLfloat body_spec[] = {0.45f, 0.45f, 0.45f, 1}; // Specular redup
    GLfloat body_emi[] = {0, 0, 0, 1};  // Emission: no glow normally

    // Frightened mode: tambah blue emission glow
    if (frightened)
        body_emi[2] = 0.25f;

    // Apply material properties ke OpenGL
    glMaterialfv(GL_FRONT, GL_AMBIENT, body_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, body_spec);
    glMaterialfv(GL_FRONT, GL_EMISSION, body_emi);
    glMaterialf(GL_FRONT, GL_SHININESS, 60); // Shininess = 60 (specular highlight)

    // ============== DRAW BODY ==============
    // Main body: cylinder + sphere pada top
    glPushMatrix();
    glRotatef(-90, 1, 0, 0); // Rotate cylinder ke atas (Y axis)
    GLUquadric *q = gluNewQuadric();
    gluCylinder(q, 1.0, 1.0, 1.5, 24, 8); // radius=1.0, height=1.5, segments=24,8
    gluDeleteQuadric(q);
    glPopMatrix();

    // Head: sphere di atas cylinder
    glPushMatrix();
    glTranslatef(0, 1.5f, 0);
    glutSolidSphere(1.0f, 24, 24); // radius=1.0, resolution=24x24
    glPopMatrix();

    // ============== DRAW BOTTOM WAVY PARTS (GHOST SKIRT) ==============
    // 5 bumpy parts di bawah untuk ghost look
    for (int b = 0; b < 5; b++)
    {
        float bx = -1.0f + b * 0.5f; // X position: -1.0, -0.5, 0.0, 0.5, 1.0

        // Bobbing animation: sine wave untuk up/down motion
        float bob = sinf(t * 4.0f + b * 1.2f) * 0.05f;

        glPushMatrix();
        glTranslatef(bx, -0.3f + bob, 0); // Posisi + bobbing
        glScalef(0.22f, 0.35f, 0.22f);    // Scale kecil untuk bump effect
        glutSolidSphere(1.0f, 10, 10);    // Small sphere
        glPopMatrix();
    }

    // ============== DRAW EYES ==============
    // Mata merah menyala agar hantu terlihat lebih mengancam.
    GLfloat eye_diff[] = {1.0f, 0.05f, 0.02f, 1};
    GLfloat eye_amb[] = {0.35f, 0.0f, 0.0f, 1};
    GLfloat eye_emi[] = {0.55f, 0.0f, 0.0f, 1};
    GLfloat noEmi[] = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, eye_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, eye_diff);
    glMaterialfv(GL_FRONT, GL_EMISSION, eye_emi);
    glMaterialf(GL_FRONT, GL_SHININESS, 80);

    // Draw 2 eye white (radius=0.28, posisi x=-0.38 dan +0.38)
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.38f, 1.55f, 0.88f); // Left/right eye position
        glScalef(1.15f, 0.72f, 0.85f);
        glutSolidSphere(0.28f, 16, 16);
        glPopMatrix();
    }

    // ============== DRAW PUPILS ==============
    // Pupil: 2 bola hitam kecil di depan mata merah
    GLfloat pupil_diff[] = {0, 0, 0, 1}; // Black diffuse
    GLfloat pupil_amb[] = {0, 0, 0, 1};  // Black ambient
    glMaterialfv(GL_FRONT, GL_AMBIENT, pupil_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pupil_diff);

    // Draw 2 pupils (radius=0.13, sedikit di depan eyes di Z=1.14)
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.38f, 1.55f, 1.14f); // Pupil position (di depan mata)
        glScalef(0.65f, 1.35f, 0.65f);
        glutSolidSphere(0.12f, 12, 12);
        glPopMatrix();
    }

    // ============== DRAW ANGRY BROWS ==============
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

    // ============== DRAW MOUTH & FANGS ==============
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

    // ============== DRAW HEAD SPIKES ==============
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

    // ============== RESTORE POLYGON MODE ==============
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPopMatrix(); // Pop transformation
}
