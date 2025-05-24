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

// --- El programa ---
std::vector<Point> contornoGatoB = {
    { -0.353975f, -0.820921f },
    { -0.32887f, -0.878661f },
    { -0.32887f, -0.878661f },
    { -0.524686f, -0.896234f },
    { -0.524686f, -0.896234f },
    { -0.680335f, -0.803347f },
    { -0.680335f, -0.803347f },
    { -0.612552f, -0.682845f },
    { -0.612552f, -0.682845f },
    { -0.3841f, -0.632636f },
    { -0.3841f, -0.632636f },
    { -0.21841f, -0.630126f },
    { -0.21841f, -0.630126f },
    { -0.278661f, -0.479498f },
    { -0.278661f, -0.479498f },
    { -0.21841f, 0.0125523f },
    { -0.21841f, 0.0125523f },
    { -0.097908f, 0.205858f },
    { -0.097908f, 0.205858f },
    { -0.097908f, 0.246025f },
    { -0.097908f, 0.246025f },
    { -0.0476987f, 0.421757f },
    { -0.0476987f, 0.421757f },
    { -0.155649f, 0.431799f },
    { -0.155649f, 0.431799f },
    { -0.251046f, 0.512134f },
    { -0.251046f, 0.512134f },
    { -0.251046f, 0.542259f },
    { -0.251046f, 0.542259f },
    { -0.208368f, 0.612552f },
    { -0.208368f, 0.612552f },
    { -0.2159f, 0.665272f },
    { -0.2159f, 0.665272f },
    { -0.198326f, 0.730544f },
    { -0.198326f, 0.730544f },
    { -0.128033f, 0.803347f },
    { -0.128033f, 0.803347f },
    { -0.213389f, 0.923849f },
    { -0.213389f, 0.923849f },
    { -0.0627615f, 0.866109f },
    { -0.0627615f, 0.866109f },
    { -0.060251f, 0.938912f },
    { -0.060251f, 0.938912f },
    { 0.133054f, 0.853557f },
    { 0.133054f, 0.853557f },
    { 0.21841f, 0.841004f },
    { 0.21841f, 0.841004f },
    { 0.296234f, 0.76318f },
    { 0.296234f, 0.76318f },
    { 0.361506f, 0.627615f },
    { 0.361506f, 0.630126f },
    { 0.341423f, 0.522176f },
    { 0.341423f, 0.522176f },
    { 0.459414f, 0.351464f },
    { 0.459414f, 0.351464f },
    { 0.431799f, 0.0527197f },
    { 0.431799f, 0.0527197f },
    { 0.469456f, -0.16569f },
    { 0.469456f, -0.16569f },
    { 0.549791f, -0.313808f },
    { 0.549791f, -0.313808f },
    { 0.537238f, -0.534728f },
    { 0.537238f, -0.534728f },
    { 0.497071f, -0.687866f },
    { 0.497071f, -0.687866f },
    { 0.348954f, -0.758159f },
    { 0.348954f, -0.758159f },
    { 0.361506f, -0.785774f },
    { 0.361506f, -0.785774f },
    { 0.331381f, -0.825942f },
    { 0.331381f, -0.825942f },
    { 0.311297f, -0.81841f },
    { 0.311297f, -0.81841f },
    { 0.288703f, -0.833473f },
    { 0.288703f, -0.833473f },
    { 0.263598f, -0.8159f },
    { 0.263598f, -0.8159f },
    { 0.235983f, -0.835983f },
    { 0.235983f, -0.835983f },
    { 0.213389f, -0.81841f },
    { 0.213389f, -0.81841f },
    { 0.173222f, -0.81841f },
    { 0.173222f, -0.81841f },
    { 0.155649f, -0.828452f },
    { 0.155649f, -0.828452f },
    { 0.125523f, -0.823431f },
    { 0.125523f, -0.823431f },
    { 0.105439f, -0.835983f },
    { 0.10795f, -0.835983f },
    { 0.0853556f, -0.828452f },
    { 0.0853556f, -0.828452f },
    { 0.060251f, -0.838494f },
    { 0.060251f, -0.838494f },
    { 0.0351464f, -0.823431f },
    { 0.0351464f, -0.823431f },
    { -0.0125523f, -0.828452f },
    { -0.0125523f, -0.828452f },
    { -0.0225941f, -0.803347f },
    { -0.0225941f, -0.803347f },
    { -0.0225941f, -0.773222f },
    { -0.0225941f, -0.773222f },
    { -0.155649f, -0.755649f },
    { -0.155649f, -0.755649f },
    { -0.160669f, -0.738075f },
    { -0.160669f, -0.738075f },
    { -0.276151f, -0.76067f },
    { -0.276151f, -0.76067f },
    { -0.386611f, -0.733054f },
    { -0.386611f, -0.733054f },
    { -0.494561f, -0.775732f },
    { -0.494561f, -0.775732f },
    { -0.504603f, -0.805858f },
    { -0.504603f, -0.805858f },
    { -0.429289f, -0.835983f },
    { -0.429289f, -0.835983f },
    { -0.356485f, -0.823431f },
    { -0.356485f, -0.823431f }
};

std::vector<Point> pata1B = {
    { 0.0803347f, 0.268619f },
    { -0.060251f, 0.0276151f },
    { -0.060251f, 0.0276151f },
    { -0.0878661f, -0.243515f },
    { -0.0853556f, -0.246025f },
    { 0.0200837f, -0.59749f },
    { 0.0200837f, -0.59749f },
    { 0, -0.71046f },
    { 0, -0.71046f },
    { -0.0251046f, -0.740586f },
    { -0.0251046f, -0.740586f },
    { -0.0150628f, -0.778243f },
    { -0.023f, -0.776f }
};

std::vector<Point> pata2B = {
    { 0.133054f, -0.0200837f },
    { 0.155649f, -0.682845f },
    { 0.155649f, -0.682845f },
    { 0.208368f, -0.429289f },
    { 0.208368f, -0.429289f },
    { 0.193305f, 0.00251046f },
    { 0.193305f, 0.00251046f }
};

std::vector<Point> pata3B = {
    { 0.2159f, 0.343933f },
    { 0.303766f, 0.0753138f },
    { 0.303766f, 0.0753138f },
    { 0.401674f, -0.0878661f },
    { 0.401674f, -0.0878661f },
    { 0.409205f, -0.205858f },
    { 0.409205f, -0.208368f },
    { 0.306276f, -0.685356f },
    { 0.306276f, -0.685356f },
    { 0.348954f, -0.76067f },
    { 0.348954f, -0.76067f }
};

std::vector<Point> cola1B = {
    { -0.331381f, -0.876151f },
    { -0.499582f, -0.813389f },
    { -0.499582f, -0.813389f },
    { -0.529707f, -0.888703f },
    { -0.529707f, -0.888703f },
    { -0.675314f, -0.805858f },
    { -0.675314f, -0.805858f },
    { -0.504603f, -0.810879f },
    { -0.504603f, -0.810879f },
    { -0.582427f, -0.717992f },
    { -0.582427f, -0.717992f },
    { -0.677824f, -0.803347f },
    { -0.677824f, -0.803347f },
    { -0.642678f, -0.740586f },
    { -0.642678f, -0.740586f },
    { -0.584937f, -0.717992f },
    { -0.584937f, -0.717992f },
    { -0.610042f, -0.682845f },
    { -0.610042f, -0.682845f },
    { -0.487029f, -0.705439f },
    { -0.487029f, -0.705439f },
    { -0.584937f, -0.712971f },
    { -0.584937f, -0.712971f },
    { -0.527197f, -0.750628f },
    { -0.527197f, -0.750628f },
    { -0.504603f, -0.803347f },
    { -0.504603f, -0.803347f },
    { -0.487029f, -0.70795f },
    { -0.487029f, -0.70795f },
    { -0.386611f, -0.733054f },
    { -0.386611f, -0.733054f },
    { -0.37908f, -0.640167f },
    { -0.37908f, -0.640167f },
    { -0.484519f, -0.705439f },
    { -0.484519f, -0.705439f }
};

std::vector<Point> cola2B = {
    { -0.37908f, -0.637657f },
    { -0.27113f, -0.758159f },
    { -0.27113f, -0.758159f },
    { -0.16569f, -0.740586f },
    { -0.16569f, -0.740586f },
    { -0.374059f, -0.640167f },
    { -0.374059f, -0.640167f },
    { -0.21841f, -0.627615f },
    { -0.21841f, -0.627615f },
    { -0.16318f, -0.735565f },
    { -0.16318f, -0.735565f }
};

std::vector<Point> cuerpo1B = {
    { -0.0225941f, -0.76569f },
    { -0.276151f, -0.471967f },
    { -0.276151f, -0.471967f },
    { -0.0803347f, -0.175732f },
    { -0.0803347f, -0.175732f },
    { -0.213389f, 0.0100418f },
    { -0.213389f, 0.0100418f },
    { -0.097908f, 0.208368f },
    { -0.097908f, 0.208368f },
    { -0.0778243f, -0.102929f },
    { -0.0778243f, -0.102929f },
    { -0.060251f, 0.0251046f },
    { -0.060251f, 0.0251046f },
    { -0.0828452f, -0.00753138f },
    { -0.0828452f, -0.00753138f }
};

std::vector<Point> cuerpo2B = {
    { -0.100418f, 0.241004f },
    { 0.0803347f, 0.268619f },
    { 0.0803347f, 0.268619f },
    { -0.0476987f, 0.414226f },
    { -0.0476987f, 0.414226f },
    { 0.060251f, 0.376569f },
    { 0.060251f, 0.376569f },
    { 0.123013f, 0.414226f },
    { 0.123013f, 0.414226f },
    { 0.0803347f, 0.27364f },
    { 0.0803347f, 0.27364f },
    { 0.208368f, 0.268619f },
    { 0.208368f, 0.268619f },
    { 0.128033f, 0.411716f },
    { 0.128033f, 0.411716f },
    { 0.210879f, 0.343933f },
    { 0.210879f, 0.343933f },
    { 0.276151f, 0.632636f },
    { 0.276151f, 0.632636f },
    { 0.125523f, 0.419247f },
    { 0.125523f, 0.419247f },
    { 0.0778243f, 0.537239f },
    { 0.0778243f, 0.537239f },
    { 0.0200837f, 0.421757f },
    { 0.0200837f, 0.421757f },
    { -0.0502092f, 0.424268f },
    { -0.0502092f, 0.424268f },
    { 0.0753138f, 0.537239f },
    { 0.0753138f, 0.537239f },
    { 0.0803347f, 0.660251f },
    { 0.0803347f, 0.660251f },
    { 0.361506f, 0.630126f },
    { 0.361506f, 0.630126f },
    { 0.203347f, 0.740586f },
    { 0.203347f, 0.740586f },
    { 0.0803347f, 0.662762f },
    { 0.0803347f, 0.662762f },
    { 0.0426778f, 0.612552f },
    { 0.0426778f, 0.612552f },
    { 0.0803347f, 0.539749f },
    { 0.0803347f, 0.539749f },
    { -0.0401674f, 0.552301f },
    { -0.0376569f, 0.552301f },
    { 0.0426778f, 0.607531f },
    { 0.0426778f, 0.607531f },
    { -0.0476987f, 0.657741f },
    { -0.0476987f, 0.657741f },
    { -0.0376569f, 0.552301f },
    { -0.0376569f, 0.552301f },
    { -0.0502092f, 0.43431f },
    { -0.0502092f, 0.43431f },
    { -0.175732f, 0.459414f },
    { -0.175732f, 0.459414f },
    { -0.0953975f, 0.469456f },
    { -0.0953975f, 0.469456f },
    { -0.0527197f, 0.429289f },
    { -0.0527197f, 0.429289f }
};

Figure contornoGato = genBezier(contornoGatoB);
Figure pata1 = genBezier(pata1B);
Figure pata2 = genBezier(pata2B);
Figure pata3 = genBezier(pata3B);
Figure cola1 = genBezier(cola1B);
Figure cola2 = genBezier(cola2B);
Figure cuerpo1 = genBezier(cuerpo1B);
Figure cuerpo2 = genBezier(cuerpo2B);

void drawShape()
{
    draw(BORDER, contornoGato, 4);
    draw(BORDER, pata1, 4);
    draw(BORDER, pata2, 4);
    draw(BORDER, pata3, 4);
    draw(BORDER, cola1, 2);
    draw(BORDER, cola2, 2);
    draw(BORDER, cuerpo1, 2);
    draw(BORDER, cuerpo2, 2);
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
    glutCreateWindow("Gato");

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
