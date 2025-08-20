/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/*
 *  light.c
 *  This program demonstrates the use of the OpenGL lighting
 *  model.  A sphere is drawn using a grey material characteristic.
 *  A single light source illuminates the object.
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* --- estado das luzes e ângulos de rotação --- */
static int light0_on = 1;  // luz vermelha ligada inicialmente
static int light1_on = 1;  // luz verde ligada inicialmente
static int light2_on = 1;  // luz azul ligada inicialmente

/* não uso animação automática (idle); só giro quando clico */
static float angleX = 0.0f;     // ângulo da luz vermelha (rotação no eixo X)
static float angleY = 180.0f;   // ângulo da luz verde   (rotação no eixo Y)
static float angleZ = 0.0f;     // ângulo da luz azul    (rotação no eixo Z)

static float light_radius = 3.0f;  // raio da "órbita" das luzes

/* variáveis pra girar o objeto com o mouse (clique e arrasta) */
static int dragging = 0;
static int down_x = 0, down_y = 0;   // onde apertei o botão
static int last_x = 0, last_y = 0;   // última posição no movimento
static float obj_rot_x = 20.0f, obj_rot_y = 30.0f;  // ângulos iniciais do objeto

/* cada clique vai girar as luzes de um certo tanto */
#define CLICK_DELTA 10.0f

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double deg2rad(double d) { return d * M_PI / 180.0; }

void init(void)
{
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { 50.0f };
    GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };

    glClearColor (0.1f, 0.1f, 0.1f, 1.0f);  // fundo escuro

    glShadeModel (GL_SMOOTH);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // vermelho
    glEnable(GL_LIGHT1); // verde
    glEnable(GL_LIGHT2); // azul

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

/* aqui calculo a posição das três luzes e passo pro OpenGL */
void update_lights(void)
{
    GLfloat pos0[4], diff0[4], spec0[4];
    GLfloat pos1[4], diff1[4], spec1[4];
    GLfloat pos2[4], diff2[4], spec2[4];

    // vermelha gira em torno de X
    pos0[0] = 0.0f;
    pos0[1] = light_radius * cosf((float)deg2rad(angleX));
    pos0[2] = light_radius * sinf((float)deg2rad(angleX));
    pos0[3] = 1.0f;
    diff0[0] = 1.0f; diff0[1] = 0.2f; diff0[2] = 0.2f; diff0[3] = 1.0f;
    spec0[0] = spec0[1] = spec0[2] = spec0[3] = 1.0f;

    // verde gira em torno de Y
    pos1[0] = light_radius * cosf((float)deg2rad(angleY));
    pos1[1] = 0.0f;
    pos1[2] = light_radius * sinf((float)deg2rad(angleY));
    pos1[3] = 1.0f;
    diff1[0] = 0.2f; diff1[1] = 1.0f; diff1[2] = 0.2f; diff1[3] = 1.0f;
    spec1[0] = spec1[1] = spec1[2] = spec1[3] = 1.0f;

    // azul gira em torno de Z
    pos2[0] = light_radius * cosf((float)deg2rad(angleZ));
    pos2[1] = light_radius * sinf((float)deg2rad(angleZ));
    pos2[2] = 0.0f;
    pos2[3] = 1.0f;
    diff2[0] = 0.2f; diff2[1] = 0.2f; diff2[2] = 1.0f; diff2[3] = 1.0f;
    spec2[0] = spec2[1] = spec2[2] = spec2[3] = 1.0f;

    glLightfv(GL_LIGHT0, GL_POSITION, pos0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

    glLightfv(GL_LIGHT1, GL_POSITION, pos1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);

    glLightfv(GL_LIGHT2, GL_POSITION, pos2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diff2);
    glLightfv(GL_LIGHT2, GL_SPECULAR, spec2);

    // só liga as que estiverem marcadas como "on"
    if (light0_on) glEnable(GL_LIGHT0); else glDisable(GL_LIGHT0);
    if (light1_on) glEnable(GL_LIGHT1); else glDisable(GL_LIGHT1);
    if (light2_on) glEnable(GL_LIGHT2); else glDisable(GL_LIGHT2);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 6.0,   // olho
              0.0, 0.0, 0.0,   // centro
              0.0, 1.0, 0.0);  // "cima"

    update_lights();

    // desenho da esfera central (objeto alvo)
    glPushMatrix();
        glRotatef(obj_rot_x, 1.0f, 0.0f, 0.0f);
        glRotatef(obj_rot_y, 0.0f, 1.0f, 0.0f);
        glColor3f(0.9f, 0.9f, 0.9f);
        glutSolidSphere(1.0, 50, 40);
    glPopMatrix();

    // agora desenhar marcadores das luzes (bolinhas coloridas)
    glDisable(GL_LIGHTING);

    glPushMatrix();
        glTranslatef(0.0f, light_radius * cosf((float)deg2rad(angleX)),
                           light_radius * sinf((float)deg2rad(angleX)));
        glColor3f(1.0f, 0.2f, 0.2f);
        glutSolidSphere(0.08, 12, 12);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(light_radius * cosf((float)deg2rad(angleY)),
                     0.0f,
                     light_radius * sinf((float)deg2rad(angleY)));
        glColor3f(0.2f, 1.0f, 0.2f);
        glutSolidSphere(0.08, 12, 12);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(light_radius * cosf((float)deg2rad(angleZ)),
                     light_radius * sinf((float)deg2rad(angleZ)),
                     0.0f);
        glColor3f(0.2f, 0.2f, 1.0f);
        glutSolidSphere(0.08, 12, 12);
    glPopMatrix();

    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w/(GLdouble)h, 1.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            dragging = 1;
            down_x = x; down_y = y;
            last_x = x; last_y = y;
        } else {
            int dx = abs(x - down_x);
            int dy = abs(y - down_y);
            dragging = 0;
            if (dx < 4 && dy < 4) {
                // clique curto → ajusta os ângulos das luzes
                angleX += CLICK_DELTA;
                angleY += CLICK_DELTA * 0.9f;
                angleZ += CLICK_DELTA * 1.1f;
                if (angleX >= 360.0f) angleX -= 360.0f;
                if (angleY >= 360.0f) angleY -= 360.0f;
                if (angleZ >= 360.0f) angleZ -= 360.0f;
                printf("Clique: angles -> X: %.1f, Y: %.1f, Z: %.1f\n",
                       angleX, angleY, angleZ);
                glutPostRedisplay();
            }
        }
    }
}

void motion(int x, int y)
{
    if (!dragging) return;
    int dx = x - last_x;
    int dy = y - last_y;
    obj_rot_y += dx * 0.5f;
    obj_rot_x += dy * 0.5f;
    last_x = x; last_y = y;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27: exit(0); break;
        case '1':
            light0_on = !light0_on;
            printf("Luz vermelha %s\n", light0_on ? "ON" : "OFF");
            break;
        case '2':
            light1_on = !light1_on;
            printf("Luz verde %s\n", light1_on ? "ON" : "OFF");
            break;
        case '3':
            light2_on = !light2_on;
            printf("Luz azul %s\n", light2_on ? "ON" : "OFF");
            break;
        case 'a':
        case 'A':
            light0_on = light1_on = light2_on =
                !(light0_on && light1_on && light2_on);
            printf("Toggle all lights: %s\n", light0_on ? "ON" : "OFF");
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    printf("Controles:\n");
    printf("  Clique esquerdo (curto) -> gira as luzes\n");
    printf("  Arrastar botão esquerdo -> gira a esfera (inspeção)\n");
    printf("  1 - toggle luz vermelha\n");
    printf("  2 - toggle luz verde\n");
    printf("  3 - toggle luz azul\n");
    printf("  a - toggle todas\n");
    printf("  ESC - sair\n");

    glutMainLoop();
    return 0;
}
