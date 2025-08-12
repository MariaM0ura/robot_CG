#include <GL/glut.h>
#include <stdlib.h>
#include <math.h> // para sin e cos

// Variáveis de ângulo para cada junta
static int base = 0;
static int shoulder = 0;
static int elbow = 0;
static int wrist = 0;
static float finger = 0;

// Variáveis da câmera
static float cam_angle = 1.0; 
static float cam_dist = 8.0; 

// Limites da garra
#define FINGER_MAX 30.0
#define FINGER_MIN 0.0

// Variáveis para animação
static bool animating = false;
static int current_step = 0;
static bool gripped = false;
static float obj_pos[3] = {3.0f, -1.75f, 0.0f};
static float box_pos[3] = {0.0f, -1.75f, 3.0f};
static int target_base = 0;
static int target_shoulder = 0;
static int target_elbow = 0;
static int target_wrist = 0;
static float target_finger = 0.0;

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
}

void drawCube(float r, float g, float b, float sx, float sy, float sz) {
    glPushMatrix();
    glColor3f(r, g, b);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glColor3f(1.0, 1.0, 1.0);
    glutWireCube(1.0);
    glPopMatrix();
}

void get_ee_position(float *pos) {
    float theta1 = shoulder * M_PI / 180.0;
    float theta2 = elbow * M_PI / 180.0;
    float theta3 = wrist * M_PI / 180.0;
    float b = base * M_PI / 180.0;
    float x_plane = 2 * cos(theta1) + 2 * cos(theta1 + theta2) + 1 * cos(theta1 + theta2 + theta3);
    float y = 0.25 + 2 * sin(theta1) + 2 * sin(theta1 + theta2) + 1 * sin(theta1 + theta2 + theta3);
    pos[0] = x_plane * cos(b);
    pos[1] = y;
    pos[2] = x_plane * sin(b);
}

void animate(int value) {
    int step_size = 2; // graus por frame para juntas
    float finger_step = 1.0; // para finger

    if (base < target_base) base += step_size;
    else if (base > target_base) base -= step_size;

    if (shoulder < target_shoulder) shoulder += step_size;
    else if (shoulder > target_shoulder) shoulder -= step_size;

    if (elbow < target_elbow) elbow += step_size;
    else if (elbow > target_elbow) elbow -= step_size;

    if (wrist < target_wrist) wrist += step_size;
    else if (wrist > target_wrist) wrist -= step_size;

    if (finger < target_finger) finger += finger_step;
    else if (finger > target_finger) finger -= finger_step;

    // Verificar se alcançou os alvos
    bool reached = (abs(base - target_base) <= step_size &&
                    abs(shoulder - target_shoulder) <= step_size &&
                    abs(elbow - target_elbow) <= step_size &&
                    abs(wrist - target_wrist) <= step_size &&
                    fabs(finger - target_finger) <= finger_step);

    if (reached) {
        switch (current_step) {
            case 0:
                // Abrir garra
                target_finger = FINGER_MIN;
                current_step++;
                break;
            case 1:
                // Mover para pré-pick
                target_base = 0;
                target_shoulder = 0;
                target_elbow = -60;
                target_wrist = 60;
                current_step++;
                break;
            case 2:
                // Baixar para pick
                target_elbow = -90;
                target_wrist = 90;
                current_step++;
                break;
            case 3:
                // Fechar garra
                target_finger = FINGER_MAX;
                current_step++;
                break;
            case 4:
                // Agarrar
                gripped = true;
                current_step++;
                break;
            case 5:
                // Levantar
                target_elbow = -60;
                target_wrist = 60;
                current_step++;
                break;
            case 6:
                // Ir para home
                target_base = 0;
                target_shoulder = 0;
                target_elbow = 0;
                target_wrist = 0;
                current_step++;
                break;
            case 7:
                // Rotacionar base para -90
                target_base = -90;
                current_step++;
                break;
            case 8:
                // Mover para pré-place
                target_shoulder = 0;
                target_elbow = -60;
                target_wrist = 60;
                current_step++;
                break;
            case 9:
                // Baixar para place
                target_elbow = -90;
                target_wrist = 90;
                current_step++;
                break;
            case 10:
                // Abrir garra
                target_finger = FINGER_MIN;
                current_step++;
                break;
            case 11:
                // Soltar
                gripped = false;
                get_ee_position(obj_pos);
                current_step++;
                break;
            case 12:
                // Levantar
                target_elbow = -60;
                target_wrist = 60;
                current_step++;
                break;
            case 13:
                // Voltar para home
                target_shoulder = 0;
                target_elbow = 0;
                target_wrist = 0;
                current_step++;
                break;
            case 14:
                // Fim da animação
                animating = false;
                break;
        }
    }

    glutPostRedisplay();
    if (animating) {
        glutTimerFunc(20, animate, 0);
    }
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float camX = cam_dist * sin(cam_angle);
    float camZ = cam_dist * cos(cam_angle);
    gluLookAt(camX, 3.0, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Alterado y para 3.0

    // Desenhar caixa
    glPushMatrix();
    glTranslatef(box_pos[0], box_pos[1], box_pos[2]);
    drawCube(0.0, 0.0, 1.0, 1.5, 1.5, 1.5);
    glPopMatrix();

    // Desenhar objeto se não agarrado
    if (!gripped) {
        glPushMatrix();
        glTranslatef(obj_pos[0], obj_pos[1], obj_pos[2]);
        drawCube(1.0, 0.0, 0.0, 0.5, 0.5, 0.5);
        glPopMatrix();
    }

    glPushMatrix();

    // Rotação da base
    glRotatef((GLfloat)base, 0.0, 1.0, 0.0);
    drawCube(0.5, 0.5, 0.5, 2.0, 0.5, 2.0); // base

    // Ombro
    glTranslatef(0.0, 0.25, 0.0);
    glRotatef((GLfloat)shoulder, 0.0, 0.0, 1.0);
    glTranslatef(1.0, 0.0, 0.0);
    drawCube(0.8, 0.0, 0.0, 2.0, 0.4, 0.4); // braço superior

    // Cotovelo
    glTranslatef(1.0, 0.0, 0.0);
    glRotatef((GLfloat)elbow, 0.0, 0.0, 1.0);
    glTranslatef(1.0, 0.0, 0.0);
    drawCube(0.0, 0.0, 0.8, 2.0, 0.4, 0.4); // antebraço

    // Punho
    glTranslatef(1.0, 0.0, 0.0);
    glRotatef((GLfloat)wrist, 0.0, 0.0, 1.0);
    glTranslatef(0.5, 0.0, 0.0);
    drawCube(0.0, 0.8, 0.0, 1.0, 0.3, 0.3); // punho

    // Desenhar objeto se agarrado
    if (gripped) {
        glPushMatrix();
        glTranslatef(0.5, 0.0, 0.0);
        drawCube(1.0, 0.0, 0.0, 0.5, 0.5, 0.5);
        glPopMatrix();
    }

    // Dedos
    glPushMatrix();
    glTranslatef(0.5, 0.1, 0.0);
    glRotatef((GLfloat)finger, 0.0, 0.0, 1.0);
    drawCube(0.8, 0.8, 0.0, 0.6, 0.1, 0.2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.5, -0.1, 0.0);
    glRotatef((GLfloat)-finger, 0.0, 0.0, 1.0);
    drawCube(0.8, 0.8, 0.0, 0.6, 0.1, 0.2);
    glPopMatrix();

    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (GLfloat) w / (GLfloat) h, 1.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'b': base = (base + 5) % 360; break;
        case 'B': base = (base - 5) % 360; break;
        case 's': shoulder = (shoulder + 5) % 360; break;
        case 'S': shoulder = (shoulder - 5) % 360; break;
        case 'e': elbow = (elbow + 5) % 360; break;
        case 'E': elbow = (elbow - 5) % 360; break;
        case 'w': wrist = (wrist + 5) % 360; break;
        case 'W': wrist = (wrist - 5) % 360; break;

        case 'f':
            if (finger < FINGER_MAX) finger += 2.0;
            break;
        case 'F':
            if (finger > FINGER_MIN) finger -= 2.0;
            break;

        case 'p':
            animating = true;
            current_step = 0;
            target_base = base;
            target_shoulder = shoulder;
            target_elbow = elbow;
            target_wrist = wrist;
            target_finger = finger;
            glutTimerFunc(20, animate, 0);
            break;

        case 27: exit(0); break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:  cam_angle -= 0.05; break;
        case GLUT_KEY_RIGHT: cam_angle += 0.05; break;
        case GLUT_KEY_UP:    if (cam_dist > 3) cam_dist -= 0.2; break;
        case GLUT_KEY_DOWN:  cam_dist += 0.2; break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Robo Articulado com Camera e Garra Manual");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}
