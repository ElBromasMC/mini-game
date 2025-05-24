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

std::vector<Point> center1B = {
    { 0.0577406f, -0.0200837f },
    { 0.160669f, -0.0150628f },
    { 0.210879f, 0.0677824f },
    { 0.115481f, 0.100418f },
    { 0.0376569f, 0.0476988f }
};

std::vector<Point> center2B = {
    { 0.21841f, -0.0702929f },
    { 0.311297f, 0.0200837f },
    { 0.321339f, 0.102929f },
    { 0.241004f, 0.180753f },
    { 0.135565f, 0.180753f }
};

std::vector<Point> center3B = {
    { 0.266109f, -0.0903766f },
    { 0.361506f, 0 },
    { 0.374059f, 0.115481f },
    { 0.298745f, 0.210879f },
    { 0.168201f, 0.223431f }
};

std::vector<Point> center4B = {
    { 0.27364f, -0.178243f },
    { 0.401674f, -0.190795f },
    { 0.474477f, -0.155649f },
    { 0.439331f, -0.0903766f },
    { 0.32636f, -0.0200837f }
};

std::vector<Point> center5B = {
    { 0.351464f, 0.0225941f },
    { 0.612552f, 0.225941f },
    { 0.459414f, 0.635146f },
    { 0.032636f, 0.65523f },
    { -0.0803347f, 0.343933f }
};

std::vector<Point> center6B = {
    { 0.351464f, 0.180753f },
    { 0.429289f, 0.323849f },
    { 0.37908f, 0.519665f },
    { 0.21841f, 0.519665f },
    { 0.0702929f, 0.391632f }
};

std::vector<Point> center7B = {
    { 0.517155f, 0.361506f },
    { 0.647699f, 0.296234f },
    { 0.504603f, 0.16318f },
    { 0.705439f, 0.143096f },
    { 0.630126f, 0.0100418f }
};

std::vector<Point> center8B = {
    { 0.522176f, 0.361506f },
    { 0.748117f, 0.451883f },
    { 0.969038f, 0.318828f },
    { 0.856067f, 0.0903766f },
    { 0.640167f, 0.0251046f }
};

std::vector<Point> center9B = {
    { 0.190795f, 0.607531f },
    { 0.230962f, 0.725523f },
    { 0.371548f, 0.723013f },
    { 0.484519f, 0.723013f },
    { 0.534728f, 0.735565f },
    { 0.554812f, 0.647699f },
    { 0.577406f, 0.574895f },
    { 0.615063f, 0.411716f },
    { 0.517155f, 0.366527f }
};

std::vector<Point> center10B = {
    { 0.728033f, 0.37908f },
    { 0.755649f, 0.632636f },
    { 0.564854f, 0.783264f },
    { 0.353975f, 0.938912f },
    { 0.100418f, 0.81841f }
};

std::vector<Point> center11B = {
    { 0.356485f, 0.820921f },
    { 0.243515f, 1.00167f },
    { 0, 1.01925f },
    { -0.258577f, 1.01674f },
    { -0.38159f, 0.8159f }
};

std::vector<Point> hoja1B = {
    { -0.125523f, 0.645188f },
    { -0.100418f, 0.866109f },
    { 0, 0.951465f },
    { 0.105439f, 0.861088f },
    { 0.133054f, 0.640167f }
};

std::vector<Point> detalles1B = {
    { -0.060251f, 0.866109f },
    { -0.00251046f, 0.841004f },
    { -0.00502092f, 0.841004f },
    { 0.060251f, 0.876151f },
    { 0.060251f, 0.876151f },
    { 0.0878661f, 0.825942f },
    { 0.0878661f, 0.825942f },
    { 0, 0.775732f },
    { 0, 0.775732f },
    { -0.0853556f, 0.820921f },
    { -0.0853556f, 0.820921f },
    { -0.102929f, 0.76569f },
    { -0.102929f, 0.76569f },
    { -0.00251046f, 0.700418f },
    { -0.00251046f, 0.700418f },
    { 0.105439f, 0.770711f },
    { 0.105439f, 0.770711f },
    { 0.120502f, 0.71046f },
    { 0.120502f, 0.71046f },
    { 0, 0.637657f },
    { 0, 0.637657f },
    { -0.112971f, 0.705439f },
    { -0.112971f, 0.705439f }
};

std::vector<Point> detalles2B = {
    { 0, 0.336402f },
    { 0, 0.220921f },
    { 0, 0.220921f }
};

std::vector<Point> detalles3B = {
    { 0.532218f, 0.733054f },
    { 0.459414f, 0.632636f },
    { 0.459414f, 0.632636f }
};

std::vector<Point> detalles4B = {
    { -0.0376569f, 0.594979f },
    { 0, 0.567364f },
    { 0, 0.567364f },
    { 0.0376569f, 0.59749f },
    { 0.0376569f, 0.59749f }
};

std::vector<Point> detalles5B = {
    { 0.00251046f, 0.948954f },
    { 0.00251046f, 0.534728f },
    { 0.00251046f, 0.534728f }
};

Figure center1 = genBezier(center1B);
Figure center2 = genBezier(center2B);
Figure center3 = genBezier(center3B);
Figure center4 = genBezier(center4B);
Figure center5 = genBezier(center5B);
Figure center6 = genBezier(center6B);
Figure center7 = genBezier(center7B);
Figure center8 = genBezier(center8B);
Figure center9 = genBezier(center9B);
Figure center10 = genBezier(center10B);
Figure center11 = genBezier(center11B);
Figure hoja1 = genBezier(hoja1B);
Figure detalles1 = genBezier(detalles1B);
Figure detalles2 = genBezier(detalles2B);
Figure detalles3 = genBezier(detalles3B);
Figure detalles4 = genBezier(detalles4B);
Figure detalles5 = genBezier(detalles5B);

void drawShape()
{
    glPushMatrix();
    glScalef(1, -1, 1);
    // Circulos externos
    drawFlower(BORDER, center11, 5, 0, 1, 1, true);
    // Circulos internos
    drawFlower(AREA, center10, 5, 0, 1, 1, true, 3, WHITE);
    drawFlower(BORDER, center10, 5, 0, 1, 1, true);
    // Hojas
    drawFlower(AREA, center8, 5, 0, 1, 1, true, 3, WHITE);
    drawFlower(BORDER, center8, 5, 0, 1, 1, true);
    // El resto
    drawFlower(BORDER, center1, 5, 0, 1, 1, true);
    drawFlower(BORDER, center2, 5, 0, 1, 1, true);
    drawFlower(BORDER, center3, 5, 0, 1, 1, true);
    drawFlower(BORDER, center7, 5, 0, 1, 1, true);
    drawFlower(BORDER, center9, 5, 0, 1, 1, true);
    glPopMatrix();
    drawFlower(BORDER, center4, 5, 0, 1, 1, false);
    drawFlower(BORDER, center5, 5, 0, 1, 1, false);
    drawFlower(BORDER, center6, 5, 0, 1, 1, false);
    drawFlower(BORDER, hoja1, 5, 0, 1, 1, false);
    drawFlower(BORDER, detalles1, 5, 0, 1, 1, false);
    drawFlower(BORDER, detalles2, 5, 0, 1, 1, false);
    drawFlower(BORDER, detalles3, 5, 0, 1, 1, false);
    drawFlower(BORDER, detalles4, 5, 0, 1, 1, false);
    drawFlower(BORDER, detalles5, 5, 0, 1, 1, false);
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
