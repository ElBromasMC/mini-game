#include <GL/glut.h>
#include <cmath>
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
    AREAFIX,
    BORDER,
    POINTS
} DrawMode;

typedef struct {
    float x;
    float y;
} Point;

// --- Constantes ---

int SEGMENTS = 100;
ColorRGB BLACK = { 0.0f, 0.0f, 0.0f };
ColorRGB YELLOW = { 1.0f, 1.0f, 0.0f };

// --- Funciones auxiliares ---

Figure newFigure(std::vector<float>& X, std::vector<float>& Y)
{
    if (X.size() != Y.size()) {
        throw std::invalid_argument("the sizes do not match");
    }
    return { X, Y, X.size() };
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
    case AREAFIX:
        glBegin(GL_POLYGON);
        glVertex2f(0, 0);
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

void drawWithScale(DrawMode mode, Figure fig, float scaleX, float scaleY, float w = 3, ColorRGB c = BLACK)
{
    glPushMatrix();
    glScalef(scaleX, scaleY, 1.0f);
    draw(mode, fig, w, c);
    glPopMatrix();
}

// --- Figuras comunes ---

Point getBezierPoint(Point p0, Point p1, Point p2, float t)
{
    Point p;
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float u2t = 2.0f * u * t;

    p.x = uu * p0.x + u2t * p1.x + tt * p2.x;
    p.y = uu * p0.y + u2t * p1.y + tt * p2.y;

    return p;
}

Figure genBezier(const std::vector<Point> points)
{
    if (points.size() < 3 || points.size() % 2 == 0) {
        return Figure {};
    }
    int n = SEGMENTS;
    int k = (points.size() - 1) / 2;
    std::vector<float> X(k * n + 1);
    std::vector<float> Y(k * n + 1);
    X[0] = points[0].x;
    Y[0] = points[0].y;
    for (size_t i = 0; i < points.size() - 1; i += 2) {
        Point p0 = points[i];
        Point p1 = points[i + 1];
        Point p2 = points[i + 2];

        for (int j = 1; j <= n; ++j) {
            float t = (float)j / n;
            Point p = getBezierPoint(p0, p1, p2, t);
            int l = n * (i / 2) + j;
            X[l] = p.x;
            Y[l] = p.y;
        }
    }
    return newFigure(X, Y);
}

Figure genCircle(float t1 = 0, float t2 = 2 * M_PI)
{
    int n = SEGMENTS;
    std::vector<float> X(n);
    std::vector<float> Y(n);
    for (int i = 0; i < n; i++) {
        float t = t1 + (t2 - t1) * i / (n - 1);
        X[i] = cosf(t);
        Y[i] = sinf(t);
    }
    return newFigure(X, Y);
}

// --- El programa ---
float r0 = 0.979;

std::vector<Point> batmanPoints = {
    { 0.000f, -0.70795f },
    { -0.243515f, -0.682845f },
    { -0.537238f, -0.650209f },
    { -0.866109f, -0.426778f },
    { -0.87364f, -0.00251046f },
    { -0.87364f, 0.474477f },
    { -0.374059f, 0.738075f },
    { -0.115481f, 0.810879f },
    { 0.000f, 0.843515f }
};

std::vector<Point> c1 = {
    { -0.364017f, 0.76067f },
    { -0.484519f, 0.48954f },
    { -0.38159f, 0.321339f },
    { -0.316318f, 0.21841f },
    { -0.208368f, 0.241004f },
    { -0.138075f, 0.268619f },
    { -0.128033f, 0.336402f },
    { -0.100418f, 0.532218f },
    { -0.100418f, 0.846025f }
};
std::vector<Point> c2 = {
    { -0.105439f, 0.828452f },
    { -0.0451883f, 0.637657f },
    { -0.032636f, 0.615063f },
    { -0.0150628f, 0.612552f },
    { 0, 0.612552f },
    { 0, 0.853557f },
    { 0, 0.87364f }
};
std::vector<Point> c3 = {
    { -0.519665f, -0.672803f },
    { -0.615063f, -0.519665f },
    { -0.567364f, -0.358996f },
    { -0.504603f, -0.228452f },
    { -0.421757f, -0.288703f },
    { -0.321339f, -0.361506f },
    { -0.303766f, -0.429289f },
    { -0.238494f, -0.587448f },
    { -0.188285f, -0.70795f }
};
std::vector<Point> c4 = {
    { -0.308787f, -0.454393f },
    { -0.278661f, -0.306276f },
    { -0.198326f, -0.306276f },
    { -0.135565f, -0.296234f },
    { -0.0853556f, -0.394142f },
    { -0.0225941f, -0.542259f },
    { 0.000f, -0.705439f },
    { -0.0225941f, -0.725523f },
    { -0.249f, -0.7285f }
};

Figure circle = genCircle();
Figure batman = genBezier(batmanPoints);
Figure co1 = genBezier(c1);
Figure co2 = genBezier(c2);
Figure co3 = genBezier(c3);
Figure co4 = genBezier(c4);

void drawBatman()
{
    draw(AREA, batman, 1, BLACK);
    draw(AREA, co1, 1, YELLOW);
    draw(AREA, co2, 1, YELLOW);
    draw(AREA, co3, 1, YELLOW);
    draw(AREA, co4, 1, YELLOW);
}

void drawShape()
{
    glScalef(1, 0.56, 1.0f);
    drawWithScale(AREA, circle, r0, r0, 1, YELLOW);
    drawWithScale(BORDER, circle, r0, r0, 9);
    drawBatman();
    glPushMatrix();
    glScalef(-1, 1, 1.0f);
    drawBatman();
    glPopMatrix();
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
    glutCreateWindow("Batman");

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
