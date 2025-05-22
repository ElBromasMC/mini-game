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
ColorRGB WHITE = { 1.0f, 1.0f, 1.0f };
ColorRGB BLACK = { 0.0f, 0.0f, 0.0f };
ColorRGB RED = { 1.0f, 0.0f, 0.0f };
ColorRGB GREEN = { 0.0f, 1.0f, 0.0f };
ColorRGB BLUE = { 0.0f, 0.0f, 1.0f };
ColorRGB ORANGE = { 0.8f, 0.5f, 0.2f };
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

void drawWithTransScale(DrawMode mode, Figure fig, float cx, float cy, float scaleX, float scaleY, float w = 3, ColorRGB c = BLACK)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glScalef(scaleX, scaleY, 1.0f);
    draw(mode, fig, w, c);
    glPopMatrix();
}

void drawFlower(DrawMode mode, Figure fig, int n, float r, float scaleX, float scaleY, bool skip = false, float w = 3, ColorRGB c = BLACK)
{
    float t1 = 0.0;
    if (skip)
        t1 = M_PI / n;
    for (int i = 0; i < n; i++) {
        glPushMatrix();
        float theta = 2 * M_PI * i / n + t1;
        glTranslatef(r * cosf(theta), r * sinf(theta), 0.0f);
        glRotatef(theta * 180.0f / M_PI, 0, 0, 1);
        glScalef(scaleX, scaleY, 1.0f);
        draw(mode, fig, w, c);
        glPopMatrix();
    }
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

Figure genPoly(int n, bool skip = false)
{
    float t1 = 0.0;
    if (skip)
        t1 = -M_PI / 2 - M_PI / n;
    std::vector<float> X(n + 1);
    std::vector<float> Y(n + 1);
    for (int i = 0; i < n + 1; i++) {
        float t = 2 * M_PI * i / n;
        X[i] = cosf(t + t1);
        Y[i] = sinf(t + t1);
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

Figure genHoja()
{
    int n = SEGMENTS;
    std::vector<float> X(2 * n);
    std::vector<float> Y(2 * n);
    for (int i = 0; i < n; i++) {
        float t = -1.0 + 2.0 * i / (n - 1);
        X[i] = t;
        Y[i] = sinf(M_PI * (t + 1) / 2);
    }
    for (int i = 0; i < n; i++) {
        float t = -1.0 + 2.0 * i / (n - 1);
        X[n + i] = -t;
        Y[n + i] = -sinf(M_PI * (t + 1) / 2);
    }
    return newFigure(X, Y);
}

Figure genCardoid(float t1 = 0, float t2 = 2 * M_PI)
{
    int n = SEGMENTS;
    float a = 0.5;
    std::vector<float> X(n);
    std::vector<float> Y(n);
    for (int i = 0; i < n; i++) {
        float t = t1 + (t2 - t1) * i / (n - 1);
        X[i] = (a - a * sinf(t)) * cosf(t);
        Y[i] = (a - a * sinf(t)) * sinf(t);
    }
    return newFigure(X, Y);
}

Figure genRose(int k, bool skip = false, float t1 = 0, float t2 = 2 * M_PI)
{
    int n = SEGMENTS;
    std::vector<float> X(n);
    std::vector<float> Y(n);
    for (int i = 0; i < n; i++) {
        float t = t1 + (t2 - t1) * i / (n - 1);
        if (skip) {
            X[i] = sinf(k * t) * cosf(t);
            Y[i] = sinf(k * t) * sinf(t);
        } else {
            X[i] = cosf(k * t) * cosf(t);
            Y[i] = cosf(k * t) * sinf(t);
        }
    }
    return newFigure(X, Y);
}

Figure genLemniscate(float t1 = 0, float t2 = 2 * M_PI)
{
    int n = SEGMENTS;
    float a = 1.0;
    std::vector<float> X(n);
    std::vector<float> Y(n);
    for (int i = 0; i < n; i++) {
        float t = t1 + (t2 - t1) * i / (n - 1);
        X[i] = a * cosf(t) / (1 + pow(sinf(t), 2));
        Y[i] = a * sinf(t) * cosf(t) / (1 + pow(sinf(t), 2));
    }
    return newFigure(X, Y);
}

float r0 = 0.992;
float r1 = 0.921;

float dd1 = 0.254;
float rr1 = 0.309 - dd1;
float dd2 = 0.472;
float rr2 = 0.522 - dd2;
float rr21 = 0.580 - dd2;
float dd3 = 0.675;
float rr3 = 0.726 - dd3;
float dd4 = 0.760;
float rr4 = 0.806 - dd4;

std::vector<Point> deco1B = {
    { 0.868619f, -0.306276f },
    { 0.768201f, -0.235983f },
    { 0.783264f, -0.175732f },
    { 0.823431f, -0.11046f },
    { 0.913808f, -0.115481f },
    { 0.913808f, -0.097908f },
    { 0.916318f, -0.100418f },
    { 0.81841f, -0.0677824f },
    { 0.803347f, -0.00251046f },
    { 0.810879f, 0.0728034f },
    { 0.913808f, 0.100418f },
    { 0.911297f, 0.117992f },
    { 0.911297f, 0.117992f },
    { 0.803347f, 0.123013f },
    { 0.785774f, 0.188285f },
    { 0.768201f, 0.251046f },
    { 0.866109f, 0.303766f }
};

std::vector<Point> deco2B = {
    { 0.881172f, -0.251046f },
    { 0.833473f, -0.220921f },
    { 0.841004f, -0.193305f },
    { 0.861088f, -0.160669f },
    { 0.903766f, -0.168201f },
    { 0.921339f, -0.105439f },
    { 0.921339f, -0.0451883f },
    { 0.87113f, -0.032636f },
    { 0.863598f, 0 },
    { 0.883682f, 0.0527197f },
    { 0.921339f, 0.0476988f },
    { 0.918828f, 0.100418f },
    { 0.906276f, 0.168201f },
    { 0.846025f, 0.168201f },
    { 0.843515f, 0.198326f },
    { 0.841004f, 0.243515f },
    { 0.883682f, 0.253557f }
};

std::vector<Point> deco3B = {
    { 0.662762f, -0.557322f },
    { 0.564854f, -0.494561f },
    { 0.552301f, -0.421757f },
    { 0.499582f, -0.238494f },
    { 0.353975f, -0.145607f },
    { 0.283682f, -0.0677824f },
    { 0.253556f, -0.0552301f },
    { 0.138075f, -0.0502092f },
    { 0.138075f, -0.0502092f },
    { 0.0502092f, -0.00251046f },
    { 0.0502092f, -0.00251046f },
    { 0.138075f, -0.0527197f },
    { 0.138075f, -0.0527197f },
    { 0.0351464f, -0.0276151f },
    { 0.0351464f, -0.0276151f },
    { 0.135565f, -0.0527197f },
    { 0.135565f, -0.0527197f },
    { 0.210879f, -0.128033f },
    { 0.210879f, -0.128033f },
    { 0.258577f, -0.145607f },
    { 0.353975f, -0.148117f },
    { 0.542259f, -0.16318f },
    { 0.667782f, -0.100418f },
    { 0.783264f, -0.0301255f },
    { 0.856067f, -0.0753138f }
};

std::vector<Point> deco4B = {
    { 0.690377f, 0.416736f },
    { 0.632636f, 0.37908f },
    { 0.612552f, 0.316318f },
    { 0.582427f, 0.246025f },
    { 0.582427f, 0.246025f },
    { 0.667782f, 0.210879f },
    { 0.667782f, 0.210879f },
    { 0.738075f, 0.180753f },
    { 0.780753f, 0.195816f }
};

std::vector<Point> deco5B = {
    { 0.750628f, -0.308787f },
    { 0.846025f, -0.351464f },
    { 0.846025f, -0.351464f }
};

Figure circle = genCircle();
Figure deco1 = genBezier(deco1B);
Figure deco2 = genBezier(deco2B);
Figure deco3 = genBezier(deco3B);
Figure deco4 = genBezier(deco4B);
Figure deco5 = genBezier(deco5B);

// --- El programa ---
void drawShape()
{
    drawWithScale(BORDER, circle, r0, r0, 4);
    drawWithScale(BORDER, circle, r1, r1, 4);
    drawFlower(BORDER, circle, 8, dd1, rr1, rr1, false, 4);
    drawFlower(BORDER, circle, 8, dd2, rr2, rr2, false, 4);
    drawFlower(BORDER, circle, 8, dd2, rr21, rr21, false, 4);
    drawFlower(BORDER, circle, 8, dd3, rr3, rr3, false, 4);
    drawFlower(BORDER, circle, 8, dd4, rr4, rr4, true, 4);
    drawFlower(BORDER, deco1, 8, 0, 1, 1, false, 4);
    drawFlower(BORDER, deco2, 8, 0, 1, 1, false, 4);
    drawFlower(BORDER, deco3, 8, 0, 1, 1, false, 4);
    drawFlower(BORDER, deco4, 8, 0, 1, 1, false, 4);
    drawFlower(BORDER, deco5, 8, 0, 1, 1, false, 4);
}

// --- Funciones de GLUT ---

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // DEBUG
    if (debug) {
        drawImageReference();
    }

    // Dibujado principal
    drawShape();

    // DEBUG
    if (debug) {
        drawAxes(1.1f, 1.5f);
        drawCoordinatesText();
        drawBezierDebug();
    }

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
    glutCreateWindow("Pregunta2-1");

    // Init

    // DEBUG
    loadTexture("problema21.png");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Register GLUT callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // DEBUG
    glutMouseFunc(mouseCallback);
    glutKeyboardFunc(keyboardCallback);
    glutMotionFunc(mouseMotionCallback);
    glutPassiveMotionFunc(mouseMotionCallback);

    glutMainLoop();
    return 0;
}
