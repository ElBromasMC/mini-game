#include <GL/glut.h>
#include <functional>
#include <math.h>
#include <stdexcept>
#include <stdlib.h>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif
#ifndef M_PI
#define M_PI 3.14159265
#endif

// --- Estructuras ---

typedef struct {
    std::vector<float> X;
    std::vector<float> Y;
    size_t size;
} Figure;

typedef struct {
    float r, g, b;
} ColorRGB;

typedef enum {
    AREA,
    BORDER,
    POINTS
} DrawMode;

typedef struct {
    float x;
    float y;
} Point;

// --- Constantes ---

ColorRGB WHITE = { 1.0f, 1.0f, 1.0f };
ColorRGB BLACK = { 0.0f, 0.0f, 0.0f };
ColorRGB RED = { 1.0f, 0.0f, 0.0f };
ColorRGB GREEN = { 0.0f, 1.0f, 0.0f };
ColorRGB BLUE = { 0.0f, 0.0f, 1.0f };
ColorRGB ORANGE = { 0.8f, 0.5f, 0.2f };

// --- Funciones auxiliares ---

Figure newFigure(std::vector<float>& X, std::vector<float>& Y)
{
    if (X.size() != Y.size()) {
        throw std::invalid_argument("the sizes do not match");
    }
    return { X, Y, X.size() };
}

std::vector<float> linSpace(float t1, float t2, int n)
{
    if (n <= 1) {
        throw std::invalid_argument("n must be greater than 1");
    }
    std::vector<float> X(n);
    for (int i = 0; i < n; i++) {
        X[i] = t1 + (t2 - t1) * i / (n - 1);
    }
    return X;
}

std::vector<float> map(std::function<float(float)> f, std::vector<float>& X)
{
    std::vector<float> Y(X.size());
    for (size_t i = 0; i < X.size(); i++) {
        Y[i] = f(X[i]);
    }
    return Y;
}

Figure pointsToFigure(std::vector<Point> points)
{
    std::vector<float> X(points.size());
    std::vector<float> Y(points.size());
    for (size_t i = 0; i < points.size(); i++) {
        X[i] = points[i].x;
        Y[i] = points[i].y;
    }
    return newFigure(X, Y);
}

// --- Funciones de dibujado ---

void draw(DrawMode mode, Figure fig, float w = 3, ColorRGB c = BLACK)
{
    glColor3f(c.r, c.g, c.b);

    switch (mode) {
    case AREA:
        glBegin(GL_POLYGON);
        break;
    case BORDER:
        glLineWidth(w);
        glBegin(GL_LINE_STRIP);
        break;
    case POINTS:
        glPointSize(w);
        glBegin(GL_POINTS);
        break;
    }

    for (size_t i = 0; i < fig.size; i++) {
        glVertex2f(fig.X[i], fig.Y[i]);
    }
    glEnd();
    glLineWidth(1.0f);
    glPointSize(1.0f);
}

void drawWithTrans(DrawMode mode, Figure fig, float cx, float cy, float w = 3,
    ColorRGB c = BLACK)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    draw(mode, fig, w, c);
    glPopMatrix();
}

void drawWithRotate(DrawMode mode, Figure fig, float angle, float w = 3, ColorRGB c = BLACK)
{
    glPushMatrix();
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    draw(mode, fig, w, c);
    glPopMatrix();
}

void drawWithScale(DrawMode mode, Figure fig, float scaleX, float scaleY, float w = 3, ColorRGB c = BLACK)
{
    glPushMatrix();
    glScalef(scaleX, scaleY, 1.0f);
    draw(mode, fig, w, c);
    glPopMatrix();
}

void drawWithTransScale(DrawMode mode, Figure fig, float cx, float cy, float scale, float w = 3, ColorRGB c = BLACK)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glScalef(scale, scale, 1.0f);
    draw(mode, fig, w, c);
    glPopMatrix();
}

void drawFlower(DrawMode mode, Figure fig, int n, float r, float scale, bool skip = false, float w = 3, ColorRGB c = BLACK)
{
    float t1 = 0.0;
    if (skip)
        t1 = M_PI / n;
    for (int i = 0; i < n; i++) {
        glPushMatrix();
        float theta = 2 * M_PI * i / n + t1;
        glTranslatef(r * cosf(theta), r * sinf(theta), 0.0f);
        glRotatef(theta * 180.0f / M_PI, 0, 0, 1);
        glScalef(scale, scale, 1.0f);
        draw(mode, fig, w, c);
        glPopMatrix();
    }
}

// --- Figuras comunes ---

Figure genPoly(int n, bool skip = false)
{
    float t1 = 0.0;
    if (skip)
        t1 = -M_PI / 2 - M_PI / n;
    std::vector<float> T = linSpace(0, 2 * M_PI, n + 1);
    std::vector<float> X = map([t1](float t) { return cosf(t + t1); }, T);
    std::vector<float> Y = map([t1](float t) { return sinf(t + t1); }, T);
    return newFigure(X, Y);
}

// --- El programa ---

void drawShape()
{
    float r0 = 1.000;
    float r1 = 0.768;
    float r2 = 0.360;
    float e1 = 0.025;

    std::vector<Point> sharpPoints = {
        { 0.151, 0.000 },
        { r2 * cosf(M_PI / 4), r2 * sinf(M_PI / 4) },
        { r1, 0.000 },
        { r2 * cosf(M_PI / 4), -r2 * sinf(M_PI / 4) },
        { 0.151, 0.000 },
    };

    std::vector<Point> linePoints = {
        { 0.000, 0.000 },
        { r1, 0.000 },
    };

    Figure triangle = genPoly(3);
    Figure square_alt = genPoly(4, true);
    Figure circle = genPoly(100);
    Figure sharp = pointsToFigure(sharpPoints);
    Figure line = pointsToFigure(linePoints);

    // Cuerpo principal
    drawWithScale(BORDER, circle, r0, r0);
    drawWithScale(BORDER, square_alt, r2, r2, 4);
    drawWithScale(BORDER, circle, r1, r1);
    // Detalles
    drawFlower(BORDER, circle, 8, (r0 + r1) / 2, (r0 - r1) / 2 - e1);
    drawFlower(BORDER, sharp, 4, 0, 1.0);
    drawFlower(BORDER, line, 4, 0, 1.0, true);
}

// --- Funciones de GLUT ---

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Dibujado principal
    drawShape();

    glDisable(GL_BLEND);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Mantener relacion 1/1
    float aspect = (float)w / (float)h;
    float viewExtent = 1.2f;
    if (w >= h) {
        gluOrtho2D(-viewExtent * aspect, viewExtent * aspect, -viewExtent,
            viewExtent);
    } else {
        gluOrtho2D(-viewExtent, viewExtent, -viewExtent / aspect,
            viewExtent / aspect);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Main");

    // Init
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Register GLUT callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
