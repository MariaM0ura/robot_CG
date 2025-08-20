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
#define _POSIX_C_SOURCE 199309L
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int slices = 20;
static int stacks = 16;
static GLenum shade = GL_SMOOTH;

void init(void)
{
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    glClearColor (0.0, 0.0, 0.0, 0.0);

    glShadeModel (shade);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
}

static double timespec_diff_ms(const struct timespec *a, const struct timespec *b)
{
    double sec = (double)(b->tv_sec - a->tv_sec);
    double nsec = (double)(b->tv_nsec - a->tv_nsec);
    return sec * 1000.0 + nsec / 1e6;
}

void display(void)
{
    struct timespec t0, t1;
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /* posiciona a câmera / objeto para ver a esfera */
    glTranslatef(0.0f, 0.0f, -3.0f);
    glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);

    /* mede apenas o tempo do desenho (incluindo glFinish) */
    clock_gettime(CLOCK_MONOTONIC, &t0);
    glutSolidSphere (1.0, slices, stacks);
    glFinish(); /* garante que o comando foi executado antes de medir */
    clock_gettime(CLOCK_MONOTONIC, &t1);

    double elapsed_ms = timespec_diff_ms(&t0, &t1);
    printf("Shade: %s | slices: %4d | stacks: %4d | draw time: %7.3f ms\n",
           (shade == GL_SMOOTH) ? "SMOOTH" : "FLAT  ", slices, stacks, elapsed_ms);

    glFlush ();
}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho (-1.5, 1.5, -1.5*(GLfloat)h/(GLfloat)w,
                 1.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
    else
        glOrtho (-1.5*(GLfloat)w/(GLfloat)h,
                 1.5*(GLfloat)w/(GLfloat)h, -1.5, 1.5, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27: /* ESC */
            exit(0);
            break;
        case 'f': /* flat */
        case 'F':
            shade = GL_FLAT;
            glShadeModel(shade);
            printf("Changed to FLAT shading\n");
            break;
        case 's': /* smooth */
        case 'S':
            shade = GL_SMOOTH;
            glShadeModel(shade);
            printf("Changed to SMOOTH shading\n");
            break;
        case '+': /* aumentar resolução (slices e stacks) */
            slices = (slices < 2000) ? slices + 10 : slices;
            stacks = (stacks < 2000) ? stacks + 10 : stacks;
            break;
        case '-': /* diminuir resolução */
            slices = (slices > 4) ? slices - 10 : slices;
            stacks = (stacks > 4) ? stacks - 10 : stacks;
            break;
        case ']': /* aumenta só stacks */
            stacks += 10;
            break;
        case '[': /* diminui só stacks */
            if (stacks > 4) stacks -= 10;
            break;
        case '>': /* aumenta só slices */
            slices += 10;
            break;
        case '<': /* diminui só slices */
            if (slices > 4) slices -= 10;
            break;
        case 'r': /* reset */
            slices = 20; stacks = 16;
            shade = GL_SMOOTH;
            glShadeModel(shade);
            printf("Reset to defaults\n");
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    printf("Instrucoes:\n");
    printf("  f    -> GL_FLAT\n");
    printf("  s    -> GL_SMOOTH\n");
    printf("  + / -  -> aumenta/diminui slices e stacks (ambos)\n");
    printf("  > / <  -> aumenta/diminui apenas slices\n");
    printf("  ] / [  -> aumenta/diminui apenas stacks\n");
    printf("  r    -> reset defaults (20 x 16, SMOOTH)\n");
    printf("  ESC  -> sair\n\n");

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (700, 700);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Sphere shading/time benchmark");
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
