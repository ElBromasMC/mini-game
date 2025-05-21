#include <GL/glut.h>
#include <bits/stdc++.h>
#include <functional>
#include <math.h>
#include <stdexcept>
#include <stdlib.h>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

// DEBUG
#include "debug.cpp"

// --- Estructuras ---

typedef struct {
    std::vector<float> X;
    std::vector<float> Y;
    size_t size;
} Figure;

typedef struct {
    float r, g, b;
} ColorRGB;

typedef enum { AREA,
    BORDER,
    POINTS } DrawMode;

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

// TODO
Point getQuadraticBezierPoint(Point p0, Point p1, Point p2, float t)
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

// TODO
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

// TODO
void drawSolidBezierShape(const std::vector<Point>& points, ColorRGB fillColor,
    float borderWidth, bool withPoints)
{
    if (points.size() < 3 || points.size() % 2 == 0) {
        return;
    }
    // Dibujar los puntos de bezier
    // DEBUG
    if (debug && withPoints) {
        draw(POINTS, pointsToFigure(points), 6, BLUE);
    }

    // Dibujar los bordes
    glLineWidth(borderWidth);
    // DEBUG
    if (debug) {
        glColor4f(fillColor.r, fillColor.g, fillColor.b, 0.7);
    } else {
        glColor3f(fillColor.r, fillColor.g, fillColor.b);
    }
    glBegin(GL_LINE_STRIP);
    glVertex2f(points[0].x, points[0].y);
    for (size_t i = 0; i < points.size() - 1; i += 2) {
        Point p0 = points[i];
        Point p1 = points[i + 1];
        Point p2 = points[i + 2];

        for (int j = 1; j <= 100; ++j) {
            float t = (float)j / 100;
            Point p = getQuadraticBezierPoint(p0, p1, p2, t);
            glVertex2f(p.x, p.y);
        }
    }
    glEnd();
    glLineWidth(1.0f);
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

Figure genHoja(float h, float l)
{
    std::vector<float> T = linSpace(0, M_PI, 100);
    std::vector<float> X = map([h](float t) { return h * (2 * t / M_PI - 1); }, T);
    std::vector<float> Y = map([l](float t) { return l * sinf(t); }, T);
    std::vector<float> X1 = map([](float x) { return x; }, X);
    std::vector<float> Y1 = map([](float y) { return -y; }, Y);
    std::reverse(X1.begin(), X1.end());
    std::reverse(Y1.begin(), Y1.end());
    X.insert(X.end(), X1.begin(), X1.end());
    Y.insert(Y.end(), Y1.begin(), Y1.end());
    return newFigure(X, Y);
}

// --- El programa ---

Figure triangle = genPoly(3);
Figure square = genPoly(4, true);
Figure pentagon = genPoly(5);
Figure circle = genPoly(100);
Figure hoja = genHoja(1, 1);

// void drawShape()
//{
//     // Empezar a dibujar
//     draw(BORDER, pentagon);
//     draw(BORDER, hoja);
//     drawWithTrans(BORDER, square, 0, 0);
//     drawWithTransScale(BORDER, square, 0.731, 0.964, 0.1);
//     drawFlower(BORDER, square, 5, 1.0, 0.1);
//     std::vector<Point> segmentPoints = {
//         { -0.045f, -0.003f },
//         { 0.048f, -0.974f },
//         { -0.387f, -0.999f },
//         { -0.758f, -0.904f },
//         { -0.610f, -0.457f },
//         { -0.547f, -0.329f },
//         { -0.444f, -0.286f },
//         { -0.796f, -0.319f },
//         { -0.906f, 0.000f },
//         { -1.027f, 0.309f },
//         { -0.984f, 0.535f },
//         { -0.856f, 0.964f },
//         { -0.449f, 0.663f },
//         { -0.033f, 0.407f },
//         { -0.045f, -0.003f },
//     };
//
//     drawSolidBezierShape(segmentPoints, RED, 3.0f, true);
// }

void drawShape()
{
}

// --- Funciones de GLUT ---

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // DEBUG
    if (debug) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawImageReference();
    }

    // Dibujado principal
    drawShape();

    // DEBUG
    if (debug) {
        drawAxes(1.1f, 1.5f);
        drawCoordinatesText();
        glDisable(GL_BLEND);
    }

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
    // DEBUG
    if (!loadTexture("main.png")) {
        std::cerr << "Failed to load texture. Exiting." << std::endl;
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
        }
        exit(1);
    }
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Register GLUT callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    // DEBUG
    glutMouseFunc(mouseCallback);
    glutKeyboardFunc(keyboardCallback);

    glutMainLoop();
    return 0;
}
