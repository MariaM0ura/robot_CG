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
 *  cube.c
 *  This program demonstrates a single modeling transformation,
 *  glScalef() and a single viewing transformation, gluLookAt().
 *  A wireframe cube is rendered.
 */
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

static float blue_x  = 0.0f; // posição x do cubo azul
static int invert_order = 0; // só pra controle/printf
static int main_win = 0; /* armazenar id da janela */
static float red_z = 0.0f;   // posição z do cubo vermelho (mais perto inicialmente)
static float blue_z = -2.0f; // posição z do cubo azul (mais longe inicialmente)

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
   glEnable(GL_DEPTH_TEST); /* <-- importante: depth test ligado */
}

void drawCuboid(float x, float y, float z, float sx, float sy, float sz)
{
   glPushMatrix();
   glTranslatef(x, y, z);
   glScalef(sx, sy, sz);
   glutSolidCube(1.0f);
   glPopMatrix();
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* limpar também depth buffer */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
           
   gluLookAt(0.0, 0.0, 5.0,   /* eye */
             0.0, 0.0, 0.0,   /* center */
             0.0, 1.0, 0.0);  /* up */

   float red_x = 0.0f, red_y = 0.0f;   
   float blue_y = 0.0f;
   float sx = 1.0f, sy = 2.0f, sz = 1.0f;

   /* Podemos desenhar em ordem fixa — com depth test ligado a profundidade decide. */
   glColor3f(1.0f, 0.0f, 0.0f); // vermelho sólido
   drawCuboid(red_x, red_y, red_z, sx, sy, sz);

   glColor3f(0.0f, 0.0f, 1.0f); // azul sólido
   drawCuboid(blue_x, blue_y, blue_z, sx, sy, sz);

   glutSwapBuffers();
}

void reshape (int w, int h)
{
   if (h == 0) h = 1;
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
   glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
   printf("[keyboard] key=%c (%d)\n", (key >= 32 && key < 127) ? key : '?', key);
   switch (key) {
      case 27: /* ESC */
         exit(0);
         break;
      case 's':
         blue_x += 0.1f; 
         glutPostRedisplay();
         printf("Cubo azul movido para a direita (blue_x=%.2f)\n", blue_x);
         break;
      case 'S':
         blue_x -= 0.1f; 
         glutPostRedisplay();
         printf("Cubo azul movido para a esquerda (blue_x=%.2f)\n", blue_x);
         break;
      case 'i':
         invert_order = !invert_order;
         /* Ao invés de desenhar em ordem diferente, trocamos as Zs:
            isso faz o depth test colocar o azul na frente (sem "pintar" o vermelho). */
         {
            float tmp = blue_z;
            blue_z = red_z;
            red_z = tmp;
         }
         glutPostRedisplay();
         printf("Invertido: azul z=%.2f, vermelho z=%.2f (invert=%d)\n", blue_z, red_z, invert_order);
         break;
      case 'r':
         blue_x = 0.0f; 
         invert_order = 0;
         red_z = 0.0f;
         blue_z = -2.0f;
         glutPostRedisplay();
         printf("Posições resetadas\n");
         break;
      default:
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); /* precisa do depth buffer */
   glutInitWindowSize (500, 500);
   glutInitWindowPosition (100, 100);
   main_win = glutCreateWindow(argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}