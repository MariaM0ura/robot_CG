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

/*  checker.c
 *  This program texture maps a checkerboard image onto
 *  two rectangles.
 *
 *  If running this program on OpenGL 1.0, texture objects are
 *  not used.
 */
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"


/*	Create checkerboard texture	*/
#define	checkImageWidth 64
#define	checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

#ifdef GL_VERSION_1_1
static GLuint texName;
#endif

/* opcional: função para salvar uma imagem PPM para checar o resultado sem o GL */
void saveCheckAsPPM(const char *name) {
    FILE *f = fopen(name, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d %d\n255\n", checkImageWidth, checkImageHeight);
    for (int i = 0; i < checkImageHeight; ++i) {
        for (int j = 0; j < checkImageWidth; ++j) {
            unsigned char rgb[3] = {
                checkImage[i][j][0],
                checkImage[i][j][1],
                checkImage[i][j][2]
            };
            fwrite(rgb, 1, 3, f);
        }
    }
    fclose(f);
}

void loadTexture ( const char * filename ) {
    int width , height , nrChannels ;
    unsigned char * data = stbi_load ( filename , & width , & height , & nrChannels , 0);
    if ( data ) {
        glGenTextures (1 , & texName );
        glBindTexture ( GL_TEXTURE_2D , texName );
        // Set texture wrapping and filtering parameters
        glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S ,
        GL_REPEAT );
        glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T ,
        GL_REPEAT );
        glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER ,
        GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,
        GL_LINEAR );
        // Load the texture data ( check if it 's RGB or RGBA )
        if ( nrChannels == 3) {
            gluBuild2DMipmaps ( GL_TEXTURE_2D , GL_RGB , width ,
            height , GL_RGB , GL_UNSIGNED_BYTE , data );
        } else if ( nrChannels == 4) {
            gluBuild2DMipmaps ( GL_TEXTURE_2D , GL_RGBA , width ,
            height , GL_RGBA , GL_UNSIGNED_BYTE , data );
        }
        stbi_image_free ( data );
    } else {
        fprintf(stderr, "Failed to load texture: %s\n", filename);
    }
}

void makeCheckImage(void)
{
    int i, j, c;

    for (i = 0; i < checkImageHeight; i++) {
        for (j = 0; j < checkImageWidth; j++) {
            c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0)) ? 200 : 50); // cores diferentes
            checkImage[i][j][0] = (GLubyte) c / 2;
            checkImage[i][j][1] = (GLubyte) (255 - c); // contraste
            checkImage[i][j][2] = (GLubyte) (c / 2);
            checkImage[i][j][3] = 255;
        }
    }
}

void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   glEnable(GL_DEPTH_TEST);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName); // gera o id da textura
   glBindTexture(GL_TEXTURE_2D, texName); // ativa a textura usando o id


   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 
                0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

    loadTexture("paper.jpg");

}

void display(void)
{
    
    glColor3f(1.0, 0.0, 0.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, texName);


    //glBegin(GL_QUADS);
    
    //glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, -1.0f,  0.0f);
    //glTexCoord2f(0.0f, 10.0f); glVertex3f(-10.0f, 5.0f, -15.0f);
    //glTexCoord2f(10.0f, 10.0f); glVertex3f( 10.0f, 5.0f, -15.0f);
    //glTexCoord2f(10.0f, 0.0f); glVertex3f( 10.0f, -1.0f,  0.0f);
    
    //glEnd();

    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, 1.0, 32, 32);

    glFlush();
    glDisable(GL_TEXTURE_2D);
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 30.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -3.6);
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(250, 250);
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0; 
}
