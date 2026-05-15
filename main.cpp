#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game_common.h"
#include "map.h"
#include "effects.h"
#include "player.h"
#include "ghost.h"
#include "ui.h"

// ============== MAIN FUNCTION - ENTRY POINT ==============
// Fungsi: Initialization GLUT dan setup semua callbacks untuk game loop
int main(int argc, char **argv)
{
    // Seed random number generator dengan current time agar random berbeda setiap run
    srand((unsigned)time(NULL));

    // Initialize semua keyboard input arrays ke false (tidak ada tombol yang ditekan)
    memset(keys, 0, sizeof(keys));
    memset(arrowKeys, 0, sizeof(arrowKeys));

    // Initialize particle array (reset semua partikel agar inactive)
    memset(particles, 0, sizeof(particles));

    // ============== GLUT INITIALIZATION ==============
    // Initialize GLUT library dengan command line arguments
    glutInit(&argc, argv);

    // Set display mode: DOUBLE=double buffer, RGB=color, DEPTH=depth testing
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Set window size (1024x720 pixels)
    glutInitWindowSize(1024, 720);

    // Create window dengan title "Kucing Gila — Upgraded Edition"
    glutCreateWindow("Kucing Gila — Upgraded Edition");

    // ============== SETUP GRAPHICS ==============
    // Setup 3 light sources (ambient, player, ghost danger) dan fog effect
    setupLighting();

    // Load texture untuk jumpscare effect (file "jp.bmp")
    jumpscareTexture = loadBMPTexture("jp.bmp");

    // ============== REGISTER CALLBACK FUNCTIONS ==============
    // Register display callback - dipanggil saat layar perlu di-redraw
    glutDisplayFunc(display);

    // Register reshape callback - dipanggil saat window di-resize
    glutReshapeFunc(reshape);

    // Register keyboard callbacks untuk key press/release
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);

    // Register special keyboard callbacks untuk arrow keys
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);

    // Register mouse motion callbacks (passive = move tanpa button, active = sambil drag)
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion);

    // ============== SETUP GAME LOOP ==============
    // Register timer callback untuk update physics setiap 16ms (~60 FPS)
    glutTimerFunc(16, updatePhysics, 0);

    // ============== START GAME ==============
    // Main GLUT event loop - akan terus berjalan sampai program diexit
    glutMainLoop();

    return 0;
}
