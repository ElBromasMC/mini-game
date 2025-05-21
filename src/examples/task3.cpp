// Nombre: Diego A. Espinoza

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <math.h>
#include <stdlib.h>
#define PI 3.141592653589793f

float amarillo[3] = { 1, 1, 0 },
      rojo[3] = { 1, 0, 0 },
      verde[3] = { 0, 1, 0 },
      azul[3] = { 0, 0, 1 },
      cyan[3] = { 0, 1, 1 },
      magenta[3] = { 1, 0, 1 },
      blanco[3] = { 1, 1, 1 },
      gris[3] = { 0.5f, 0.5f, 0.5f },
      naranja[3] = { 1, 0.647f, 0 },
      rosa[3] = { 1, 0.75f, 0.8f },
      marron[3] = { 0.6f, 0.3f, 0 },
      verde_oscuro[3] = { 0, 0.5f, 0 },
      azul_oscuro[3] = { 0, 0, 0.5f },
      aqua[3] = { 0, 1, 0.5f },
      crema[3] = { 1, 0.9, 0.8 },
      negro[3] = { 0, 0, 0 };

void elipse(
    float cx, float cy,
    float rx, float ry,
    float* RGB1, float* RGB2,
    float w = 1.0f)
{
    int segments = 100;

    glColor3fv(RGB1);

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * PI * float(i) / float(segments);

        float x = rx * cosf(theta);
        float y = ry * sinf(theta);

        glVertex2f(x + cx, y + cy);
    }
    glEnd();

    glColor3fv(RGB2);
    glLineWidth(w);

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * PI * float(i) / float(segments);

        float x = rx * cosf(theta);
        float y = ry * sinf(theta);

        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void cardioide_sector(
    float x, float y,
    float a, float b,
    float t1, float t2,
    float m,
    int op,
    GLenum Modo, // (GL_LINE_STRIP, GL_POLYGON)
    float* RGB,
    int segmentos = 100)
{

    glColor3fv(RGB);
    glBegin(Modo);
    float r, vx, vy;
    if (Modo == GL_POLYGON || Modo == GL_TRIANGLE_FAN) {
        glVertex2f(x, y);
    }

    for (int i = 0; i <= segmentos; i++) {
        float t_actual = t1 + ((float)i / segmentos) * (t2 - t1);
        switch (op) {
        case 1:
            r = a - b * sinf(t_actual * m);
            vx = r * cosf(t_actual);
            vy = r * sinf(t_actual);
            break;

        case 2:
            r = a - b * cosf(t_actual * m);
            vx = r * cosf(t_actual);
            vy = r * sinf(t_actual);
            break;
        }

        glVertex2f(x + vx, y + vy);
    }
    glEnd();
}

void cardioide_sector_2(
    float x, float y,
    float a, float b,
    float t1, float t2,
    GLenum Modo,
    float* RGB,
    int segmentos = 100)
{

    glColor3fv(RGB);
    glBegin(Modo);

    glVertex2f(x, y);
    for (int i = 0; i <= segmentos; i++) {
        float t_actual = t1 + ((float)i / segmentos) * (t2 - t1);
        float r = 1 - cosf(t_actual);

        float vx = a * r * cosf(t_actual);
        float vy = b * r * sinf(t_actual);

        glVertex2f(x + vx, y + vy);
    }
    glEnd();
}

void display(void)
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    float A = 0.7, B = 0.4;

    elipse(0, 0, 0.75, 0.45, amarillo, negro, 6);
    elipse(0, 0, A, B, negro, negro);

    cardioide_sector(0, -B, 0, -A / 2, -PI / 2, 0, 2, 1, GL_POLYGON, amarillo);
    cardioide_sector(-0.225, -0.4, 0, -A * 0.515, -PI / 2, 0, 2, 1, GL_POLYGON, amarillo);
    cardioide_sector(0, -B, 0, -A / 2, 0, PI / 2, 2, 1, GL_POLYGON, amarillo);
    cardioide_sector(0.225, -0.4, 0, -A * 0.515, 0, PI / 2, 2, 1, GL_POLYGON, amarillo);
    cardioide_sector_2(-0.12, 0.4, 0.12, 0.2, PI + PI / 28, 25 * PI / 12, GL_POLYGON, amarillo);
    cardioide_sector_2(0.12, 0.4, -0.12, 0.2, 29 * PI / 28, 25 * PI / 12, GL_POLYGON, amarillo);

    // Correcciones xD
    elipse(-0.2, -0.33, 0.08, 0.08, amarillo, amarillo);
    elipse(-0.1, -0.38, 0.05, 0.05, amarillo, amarillo);
    elipse(-0.05, -0.38, 0.05, 0.05, amarillo, amarillo);
    elipse(0.2, -0.33, 0.08, 0.08, amarillo, amarillo);
    elipse(0.1, -0.38, 0.05, 0.05, amarillo, amarillo);
    elipse(0.05, -0.38, 0.05, 0.05, amarillo, amarillo);

    glColor3fv(amarillo);
    glBegin(GL_POLYGON);
    glVertex2f(-0.12, 0.41);
    glVertex2f(-0.035, 0.3);
    glVertex2f(0.035, 0.3);
    glVertex2f(0.12, 0.41);
    glEnd();

    glutSwapBuffers();
}

void inicio()
{
    int a = 1;
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-a, a, -a, a);
    glClearColor(0, 0, 0, 0);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(500, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Logo Batman");
    inicio();
    glutDisplayFunc(display);
    glutMainLoop();

    return EXIT_SUCCESS;
}
