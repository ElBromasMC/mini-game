#include <GL/gl.h>
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
ColorRGB YELLOW = { 1.0f, 0.647f, 0.0f };
ColorRGB LIGHTBLUE = { 0.529f, 0.808f, 0.922f };

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

float r0 = 1.000;
float r1 = 0.919;
float r2 = 0.633;
float r3 = 0.482;
float r4 = 0.442;
float r5 = 0.163;
float r6 = 0.138;

std::vector<Point> saliente1B = {
    { -0.117992f, 0.10795f },
    { -0.185774f, 0.183264f },
    { -0.180753f, 0.27364f },
    { -0.150628f, 0.399163f },
    { 0, 0.414226f },
    { 0, 0 },
    { 0, 0 },
};

std::vector<Point> saliente2B = {
    { -0.0953975f, 0.130544f },
    { -0.16569f, 0.233473f },
    { -0.11046f, 0.311297f },
    { -0.0552301f, 0.366527f },
    { 0, 0.364017f },
    { 0, 0 },
    { 0, 0 },
};

std::vector<Point> saliente3B = {
    { -0.0702929f, 0.145607f },
    { -0.120502f, 0.288703f },
    { 0, 0.303766f },
    { 0, 0 },
    { 0, 0 },
};

std::vector<Point> saliente1Bmod = {
    { -0.117992f, 0.10795f },
    { -0.185774f, 0.183264f },
    { -0.180753f, 0.27364f },
    { -0.150628f, 0.399163f },
    { 0, 0.414226f },
};

std::vector<Point> saliente2Bmod = {
    { -0.0953975f, 0.130544f },
    { -0.16569f, 0.233473f },
    { -0.11046f, 0.311297f },
    { -0.0552301f, 0.366527f },
    { 0, 0.364017f },
};

std::vector<Point> saliente3Bmod = {
    { -0.0702929f, 0.145607f },
    { -0.120502f, 0.288703f },
    { 0, 0.303766f },
};

std::vector<Point> espiral1B = {
    { -0.258577f, 0.170711f },
    { -0.258577f, 0.210879f },
    { -0.288703f, 0.195816f },
    { -0.313808f, 0.170711f },
    { -0.27364f, 0.148117f },
    { -0.21841f, 0.145607f },
    { -0.235983f, 0.190795f },
    { -0.268619f, 0.258577f },
    { -0.32636f, 0.213389f },
    { -0.356485f, 0.173222f },
    { -0.313808f, 0.125523f },
    { -0.235983f, 0.0903766f },
    { -0.185774f, 0.123013f },
    { -0.148117f, 0.16569f },
    { -0.210879f, 0.266109f },
    { -0.230962f, 0.311297f },
    { -0.298745f, 0.318828f }
};

std::vector<Point> espiral2B = {
    { -0.150628f, 0.0602511f },
    { -0.198326f, 0.11046f },
    { -0.256067f, 0.115481f },
    { -0.321339f, 0.11046f },
    { -0.323849f, 0.0602511f },
    { -0.313808f, 0.0150628f },
    { -0.253556f, 0.00753138f },
    { -0.188285f, 0.0175732f },
    { -0.195816f, 0.0426778f },
    { -0.223431f, 0.0853557f },
    { -0.268619f, 0.0702929f },
    { -0.286192f, 0.032636f },
    { -0.241004f, 0.0301255f },
    { -0.213389f, 0.0451883f },
    { -0.235983f, 0.0577406f }
};

std::vector<Point> circulo1B = {
    { 0.469456f, 0.123013f },
    { 0.592469f, 0.180753f },
    { 0.524686f, 0.308787f },
    { 0.449372f, 0.414226f },
    { 0.341423f, 0.343933f }
};

std::vector<Point> circulo2B = {
    { 0.461925f, 0.145607f },
    { 0.557322f, 0.195816f },
    { 0.502092f, 0.298745f },
    { 0.441841f, 0.371548f },
    { 0.361506f, 0.323849f }
};

std::vector<Point> circulo3B = {
    { 0.449372f, 0.175732f },
    { 0.519665f, 0.210879f },
    { 0.479498f, 0.286193f },
    { 0.431799f, 0.336402f },
    { 0.37908f, 0.303766f }
};

std::vector<Point> circulo31B = {
    { 0.238494f, 0.582427f },
    { 0.391632f, 0.828452f },
    { 0.632636f, 0.622594f },
    { 0.8159f, 0.3841f },
    { 0.589958f, 0.241004f }
};

std::vector<Point> circulo32B = {
    { 0.569874f, 0.283682f },
    { 0.743096f, 0.414226f },
    { 0.587448f, 0.6f },
    { 0.416736f, 0.738075f },
    { 0.286192f, 0.567364f }
};

std::vector<Point> circulo33B = {
    { 0.542259f, 0.331381f },
    { 0.65272f, 0.439331f },
    { 0.54477f, 0.562343f },
    { 0.414226f, 0.645188f },
    { 0.333891f, 0.537239f }
};

std::vector<Point> circulo41B = {
    { 0.916318f, 0.0702929f },
    { 0.98159f, 0.092887f },
    { 0.976569f, 0.16318f },
    { 0.964017f, 0.228452f },
    { 0.893724f, 0.223431f }
};

std::vector<Point> circulo42B = {
    { 0.918828f, 0.0853557f },
    { 0.974059f, 0.115481f },
    { 0.964017f, 0.16569f },
    { 0.948954f, 0.208368f },
    { 0.896234f, 0.210879f }
};

std::vector<Point> circulo43B = {
    { 0.913808f, 0.105439f },
    { 0.956485f, 0.120502f },
    { 0.946444f, 0.16318f },
    { 0.931381f, 0.198326f },
    { 0.898745f, 0.190795f }
};

std::vector<Point> espiral31B = {
    { -0.369038f, 0.841004f },
    { -0.37908f, 0.685356f },
    { -0.233473f, 0.657741f },
    { -0.123013f, 0.695398f },
    { -0.150628f, 0.800837f },
    { -0.230962f, 0.876151f },
    { -0.288703f, 0.793306f },
    { -0.283682f, 0.70795f },
    { -0.21841f, 0.730544f },
    { -0.183264f, 0.76569f },
    { -0.21841f, 0.793306f },
    { -0.256067f, 0.800837f },
    { -0.253556f, 0.770711f },
    { -0.248536f, 0.740586f },
    { -0.21841f, 0.748117f }
};

std::vector<Point> espiral32B = {
    { -0.120502f, 0.620084f },
    { -0.175732f, 0.685356f },
    { -0.158159f, 0.755649f },
    { -0.097908f, 0.835983f },
    { -0.0225941f, 0.770711f },
    { -0.00251046f, 0.697908f },
    { -0.0502092f, 0.670293f },
    { -0.117992f, 0.667782f },
    { -0.105439f, 0.730544f },
    { -0.0803347f, 0.778243f },
    { -0.0527197f, 0.728034f },
    { -0.0451883f, 0.692887f },
    { -0.0828452f, 0.702929f },
    { -0.0953975f, 0.735565f },
    { -0.0702929f, 0.730544f }
};

std::vector<Point> espiral33B = {
    { 0.128033f, 0.622594f },
    { 0.220921f, 0.758159f },
    { 0.100418f, 0.798326f },
    { 0.00502092f, 0.803347f },
    { 0.0200837f, 0.700418f },
    { 0.0878661f, 0.627615f },
    { 0.115481f, 0.712971f },
    { 0.105439f, 0.76569f },
    { 0.0577406f, 0.745607f },
    { 0.0451883f, 0.692887f },
    { 0.0853556f, 0.702929f },
    { 0.102929f, 0.725523f },
    { 0.0803347f, 0.733054f }
};

std::vector<Point> espiral34B = {
    { 0.361506f, 0.838494f },
    { 0.399163f, 0.675314f },
    { 0.233473f, 0.662762f },
    { 0.123013f, 0.705439f },
    { 0.160669f, 0.795816f },
    { 0.2159f, 0.863598f },
    { 0.278661f, 0.808368f },
    { 0.298745f, 0.748117f },
    { 0.261088f, 0.733054f },
    { 0.210879f, 0.715481f },
    { 0.195816f, 0.768201f },
    { 0.225941f, 0.808368f },
    { 0.253556f, 0.783264f },
    { 0.256067f, 0.743096f },
    { 0.220921f, 0.758159f }
};

std::vector<Point> espiralArea31B = {
    { -0.366527f, 0.838494f },
    { -0.369038f, 0.682845f },
    { -0.238494f, 0.660251f },
    { -0.183264f, 0.667782f },
    { -0.16318f, 0.700418f },
    { 0.170711f, 0.700418f },
    { 0.170711f, 0.700418f },
    { 0.233473f, 0.660251f },
    { 0.233473f, 0.660251f },
    { 0.346444f, 0.685356f },
    { 0.358996f, 0.733054f },
    { 0.376569f, 0.783264f },
    { 0.364017f, 0.838494f }
};

std::vector<Point> espiralArea32B = {
    { -0.369038f, 0.835983f },
    { 0.364017f, 0.833473f },
    { 0.364017f, 0.833473f },
    { 0.296234f, 0.901255f },
    { 0, 0.918829f },
    { -0.293724f, 0.898745f },
    { -0.369038f, 0.835983f }
};

std::vector<Point> espiralArea33B = {
    { -0.160669f, 0.71046f },
    { 0.16569f, 0.697908f },
    { 0.16569f, 0.697908f },
    { 0.128033f, 0.622594f },
    { 0.128033f, 0.622594f },
    { 0, 0.637657f },
    { -0.117992f, 0.625105f },
    { -0.158159f, 0.65523f },
    { -0.158159f, 0.70795f }
};

Figure circle = genCircle();

void drawCuarto()
{
    drawWithScale(AREA, circle, r0, r0, 0, LIGHTBLUE);
    drawWithScale(BORDER, circle, r0, r0, 2);
    int n = 19;
    for (int i = 0; i < n; i++) {
        glPushMatrix();
        float theta = 2 * M_PI * i / n;
        glRotatef(theta * 180.0f / M_PI, 0, 0, 1);
        draw(AREA, genBezier(circulo41B), 0, YELLOW);
        draw(BORDER, genBezier(circulo41B), 2);
        draw(AREA, genBezier(circulo42B), 0, RED);
        draw(BORDER, genBezier(circulo42B), 2);
        draw(AREA, genBezier(circulo43B), 0, LIGHTBLUE);
        draw(BORDER, genBezier(circulo43B), 2);
        glPopMatrix();
    }
}

void drawTercero()
{
    drawWithScale(AREA, circle, r1, r1, 0, RED);
    int n = 4;
    for (int i = 0; i < n; i++) {
        glPushMatrix();
        float theta = 2 * M_PI * i / n;
        glRotatef(theta * 180.0f / M_PI, 0, 0, 1);
        draw(AREA, genBezier(circulo31B), 0, YELLOW);
        draw(BORDER, genBezier(circulo31B), 5);
        draw(AREA, genBezier(circulo32B), 0, WHITE);
        draw(BORDER, genBezier(circulo32B), 5);
        draw(AREA, genBezier(circulo33B), 0, LIGHTBLUE);
        draw(BORDER, genBezier(circulo33B), 5);

        draw(AREA, genBezier(espiralArea31B), 0, LIGHTBLUE);
        draw(AREA, genBezier(espiralArea32B), 0, LIGHTBLUE);
        draw(AREA, genBezier(espiralArea33B), 0, LIGHTBLUE);
        draw(BORDER, genBezier(espiral31B), 5);
        draw(BORDER, genBezier(espiral32B), 4);
        draw(BORDER, genBezier(espiral33B), 4);
        draw(BORDER, genBezier(espiral34B), 5);
        glPopMatrix();
    }
    drawWithScale(BORDER, circle, r1, r1, 3);
}

void drawSegundo()
{
    drawWithScale(AREA, circle, r2, r2, 0, WHITE);
    drawWithScale(BORDER, circle, r2, r2, 5);
    int n = 6;
    for (int i = 0; i < n; i++) {
        glPushMatrix();
        float theta = 2 * M_PI * i / n;
        glRotatef(theta * 180.0f / M_PI, 0, 0, 1);
        draw(AREA, genBezier(circulo1B), 0, LIGHTBLUE);
        draw(BORDER, genBezier(circulo1B));
        draw(AREA, genBezier(circulo2B), 0, RED);
        draw(BORDER, genBezier(circulo2B));
        draw(AREA, genBezier(circulo3B), 0, YELLOW);
        draw(BORDER, genBezier(circulo3B));
        glPopMatrix();
    }
}

void drawPrimero()
{
    draw(AREA, genBezier(saliente1B), 0, RED);
    draw(BORDER, genBezier(saliente1Bmod), 5);
    draw(AREA, genBezier(saliente2B), 0, WHITE);
    draw(BORDER, genBezier(saliente2Bmod), 4);
    draw(AREA, genBezier(saliente3B), 0, YELLOW);
    draw(BORDER, genBezier(saliente3Bmod), 4);
    draw(BORDER, genBezier(espiral1B), 5);
    draw(BORDER, genBezier(espiral2B), 4);
    drawWithScale(AREA, circle, r5, r5, 4, WHITE);
    drawWithScale(BORDER, circle, r5, r5, 4);
    drawWithScale(AREA, circle, r6, r6, 0, RED);
    drawWithTransScale(AREA, circle, -0.394, 0.038, 0.022, 0.022, 0, RED);
    drawWithTransScale(AREA, circle, -0.389, 0.088, 0.01, 0.01, 0, RED);
}

void drawPrincipal()
{
    drawWithScale(AREA, circle, r3, r3, 0, LIGHTBLUE);
    drawWithScale(BORDER, circle, r3, r3, 3);
    drawWithScale(AREA, circle, r4, r4, 0, YELLOW);
    drawWithScale(BORDER, circle, r4, r4, 4);
}

void drawShape()
{
    drawCuarto();
    drawTercero();
    drawSegundo();
    drawPrincipal();
    // Primero
    drawPrimero();
    glPushMatrix();
    glScalef(1, -1, 1);
    drawPrimero();
    glPopMatrix();
    glPushMatrix();
    glScalef(-1, 1, 1);
    drawPrimero();
    glPopMatrix();
    glPushMatrix();
    glScalef(-1, -1, 1);
    drawPrimero();
    glPopMatrix();
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
    glutCreateWindow("Problema 4");

    // Init

    // DEBUG
    loadTexture("problema4.png");

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
