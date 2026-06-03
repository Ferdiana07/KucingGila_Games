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

int main(int argc, char **argv)
{
    srand((unsigned)time(NULL));

    memset(keys, 0, sizeof(keys));
    memset(arrowKeys, 0, sizeof(arrowKeys));
    memset(particles, 0, sizeof(particles));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 720);
    glutCreateWindow("Crazy Cat 3D");

    setupLighting();
    jumpscareTexture = loadBMPTexture("jp.bmp");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion);
    glutTimerFunc(16, updatePhysics, 0);

    glutMainLoop();

    return 0;
}
