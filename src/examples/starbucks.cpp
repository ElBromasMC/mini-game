// Alumno: Linares Rojas, Ander Rafael
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

float r0 = 1.000;
std::vector<Point> cuerpoB = {
    { -0.180753f, -0.98159f },
    { -0.190795f, -0.87364f },
    { -0.10795f, -0.725523f },
    { -0.0527197f, -0.625105f },
    { -0.138075f, -0.514644f },
    { -0.258577f, -0.356485f },
    { -0.188285f, -0.266109f },
    { -0.120502f, -0.193305f },
    { -0.10795f, -0.125523f },
    { -0.0953975f, -0.092887f },
    { -0.065272f, -0.11046f },
    { 0, -0.123013f },
    { 0, -0.123013f }
};
std::vector<Point> caraB = {
    { 0, -0.0677824f },
    { -0.097908f, -0.0552301f },
    { -0.168201f, 0.0677824f },
    { -0.205858f, 0.158159f },
    { -0.195816f, 0.195816f },
    { -0.190795f, 0.246025f },
    { -0.153138f, 0.21841f },
    { -0.128033f, 0.205858f },
    { -0.0753138f, 0.223431f },
    { -0.0502092f, 0.220921f },
    { -0.0376569f, 0.208368f },
    { -0.0200837f, 0.318828f },
    { -0.065272f, 0.321339f },
    { -0.138075f, 0.356485f },
    { -0.188285f, 0.32636f },
    { -0.173222f, 0.446862f },
    { 0.00251046f, 0.454393f }
};
std::vector<Point> cabello1B = {
    { -0.251046f, -0.966527f },
    { -0.256067f, -0.886193f },
    { -0.203347f, -0.783264f },
    { -0.138075f, -0.675314f },
    { -0.158159f, -0.6f },
    { -0.16569f, -0.564854f },
    { -0.223431f, -0.474477f },
    { -0.336402f, -0.333891f },
    { -0.2159f, -0.223431f },
    { -0.128033f, -0.117992f },
    { -0.175732f, -0.00251046f },
    { -0.313808f, 0.258577f },
    { -0.170711f, 0.429289f },
    { -0.241004f, 0.401674f },
    { -0.276151f, 0.308787f },
    { -0.341423f, 0.195816f },
    { -0.241004f, -0.00251046f },
    { -0.195816f, -0.0953975f },
    { -0.281172f, -0.21841f },
    { -0.404184f, -0.331381f },
    { -0.296234f, -0.474477f },
    { -0.228452f, -0.582427f },
    { -0.230962f, -0.610042f },
    { -0.220921f, -0.682845f },
    { -0.276151f, -0.770711f },
    { -0.346444f, -0.881172f },
    { -0.331381f, -0.941423f }
};
std::vector<Point> cabello2B = {
    { -0.401674f, -0.916318f },
    { -0.391632f, -0.835983f },
    { -0.32636f, -0.728034f },
    { -0.253556f, -0.625105f },
    { -0.348954f, -0.48954f },
    { -0.476987f, -0.333891f },
    { -0.316318f, -0.188285f },
    { -0.258577f, -0.11046f },
    { -0.293724f, -0.00502092f },
    { -0.414226f, 0.238494f },
    { -0.256067f, 0.406695f },
    { -0.376569f, 0.348954f },
    { -0.391632f, 0.170711f },
    { -0.386611f, 0.0401674f },
    { -0.351464f, 0 },
    { -0.306276f, -0.117992f },
    { -0.38159f, -0.185774f },
    { -0.534728f, -0.341423f },
    { -0.406695f, -0.494561f },
    { -0.316318f, -0.612552f },
    { -0.391632f, -0.720502f },
    { -0.461925f, -0.861088f },
    { -0.451883f, -0.888703f }
};
std::vector<Point> cabello3B = {
    { -0.512134f, -0.858577f },
    { -0.499582f, -0.800837f },
    { -0.426778f, -0.682845f },
    { -0.391632f, -0.607531f },
    { -0.43682f, -0.54728f },
    { -0.429289f, -0.647699f },
    { -0.48954f, -0.712971f },
    { -0.552301f, -0.795816f },
    { -0.554812f, -0.830962f }
};
std::vector<Point> cabello4B = {
    { -0.466946f, -0.48954f },
    { -0.54477f, -0.361506f },
    { -0.504603f, -0.298745f },
    { -0.369038f, -0.117992f },
    { -0.391632f, -0.0828452f },
    { -0.396653f, -0.00251046f },
    { -0.421757f, 0.0577406f },
    { -0.461925f, 0.246025f },
    { -0.336402f, 0.364017f },
    { -0.479498f, 0.27113f },
    { -0.469456f, 0.11046f },
    { -0.469456f, 0.0602511f },
    { -0.43682f, -0.0527197f },
    { -0.424268f, -0.140586f },
    { -0.527197f, -0.241004f },
    { -0.579916f, -0.323849f },
    { -0.562343f, -0.353975f },
    { -0.635146f, -0.414226f },
    { -0.728033f, -0.424268f },
    { -0.720502f, -0.461925f },
    { -0.715481f, -0.479498f },
    { -0.778243f, -0.49205f },
    { -0.833473f, -0.54728f },
    { -0.798326f, -0.59749f },
    { -0.798326f, -0.59749f },
    { -0.717992f, -0.517155f },
    { -0.695397f, -0.532218f },
    { -0.680335f, -0.572385f },
    { -0.680335f, -0.569875f },
    { -0.605021f, -0.6f },
    { -0.466946f, -0.49205f }
};
std::vector<Point> coronaB = {
    { 0, 0.499582f },
    { -0.205858f, 0.49205f },
    { -0.338912f, 0.419247f },
    { -0.456904f, 0.607531f },
    { -0.456904f, 0.607531f },
    { -0.32887f, 0.594979f },
    { -0.278661f, 0.539749f },
    { -0.333891f, 0.687866f },
    { -0.364017f, 0.705439f },
    { -0.213389f, 0.692887f },
    { -0.133054f, 0.605021f },
    { -0.0903766f, 0.720502f },
    { -0.0878661f, 0.720502f },
    { -0.185774f, 0.808368f },
    { -0.185774f, 0.808368f },
    { -0.0552301f, 0.81841f },
    { -0.0527197f, 0.81841f },
    { 0, 0.936402f },
    { 0, 0.936402f }
};
std::vector<Point> coronaintB = {
    { 0.00251046f, 0.577406f },
    { -0.102929f, 0.567364f },
    { -0.102929f, 0.567364f },
    { 0, 0.650209f },
    { 0, 0.650209f }
};
std::vector<Point> brazo1B = {
    { -0.996653f, 0.0225941f },
    { -0.901255f, 0.0301255f },
    { -0.851046f, 0.065272f },
    { -0.702929f, 0.133054f },
    { -0.665272f, 0.10795f },
    { -0.695397f, 0.0251046f },
    { -0.695397f, 0.0251046f },
    { -0.775732f, 0.0351464f },
    { -0.838494f, -0.0150628f },
    { -0.911297f, -0.0602511f },
    { -0.994142f, -0.0552301f }
};
std::vector<Point> brazo2B = {
    { -0.989121f, -0.128033f },
    { -0.913808f, -0.130544f },
    { -0.861088f, -0.0953975f },
    { -0.810879f, -0.0476988f },
    { -0.717992f, -0.0451883f },
    { -0.740586f, -0.120502f },
    { -0.740586f, -0.120502f },
    { -0.838494f, -0.148117f },
    { -0.863598f, -0.173222f },
    { -0.916318f, -0.203347f },
    { -0.97908f, -0.200837f }
};
std::vector<Point> brazo3B = {
    { -0.936402f, -0.348954f },
    { -0.866109f, -0.333891f },
    { -0.835983f, -0.306276f },
    { -0.793305f, -0.268619f },
    { -0.750628f, -0.276151f },
    { -0.745607f, -0.210879f },
    { -0.745607f, -0.210879f },
    { -0.798326f, -0.200837f },
    { -0.863598f, -0.251046f },
    { -0.911297f, -0.281172f },
    { -0.953975f, -0.283682f }
};
std::vector<Point> brazo4B = {
    { -0.876151f, -0.482008f },
    { -0.830962f, -0.461925f },
    { -0.785774f, -0.424268f },
    { -0.760669f, -0.401674f },
    { -0.735565f, -0.411716f },
    { -0.743096f, -0.353975f },
    { -0.743096f, -0.353975f },
    { -0.823431f, -0.353975f },
    { -0.838494f, -0.389121f },
    { -0.868619f, -0.421757f },
    { -0.901255f, -0.424268f }
};
std::vector<Point> brazo5B = {
    { -0.750628f, -0.660251f },
    { -0.695397f, -0.582427f },
    { -0.662762f, -0.589958f },
    { -0.645188f, -0.630126f },
    { -0.645188f, -0.630126f },
    { -0.702929f, -0.660251f },
    { -0.715481f, -0.697908f }
};
std::vector<Point> brazo6B = {
    { -0.662762f, -0.750628f },
    { -0.620084f, -0.692887f },
    { -0.605021f, -0.692887f },
    { -0.577406f, -0.723013f },
    { -0.577406f, -0.723013f },
    { -0.610042f, -0.750628f },
    { -0.625105f, -0.783264f }
};
std::vector<Point> manoB = {
    { -0.974059f, 0.105439f },
    { -0.916318f, 0.0853557f },
    { -0.773222f, 0.158159f },
    { -0.702929f, 0.185774f },
    { -0.620084f, 0.170711f },
    { -0.512134f, 0.318828f },
    { -0.431799f, 0.366527f },
    { -0.562343f, 0.411716f },
    { -0.665272f, 0.341423f },
    { -0.720502f, 0.504603f },
    { -0.780753f, 0.512134f },
    { -0.795816f, 0.356485f },
    { -0.853557f, 0.258577f },
    { -0.908787f, 0.170711f },
    { -0.974059f, 0.105439f }
};
std::vector<Point> ojoB = {
    { -0.175732f, 0.256067f },
    { -0.135565f, 0.288703f },
    { -0.0753138f, 0.263598f },
    { -0.100418f, 0.318828f },
    { -0.175732f, 0.301255f },
    { -0.203347f, 0.281172f },
    { -0.175732f, 0.256067f }
};
std::vector<Point> narizB = {
    { 0, 0.10795f },
    { -0.0276151f, 0.11046f },
    { -0.0527197f, 0.123013f },
    { -0.032636f, 0.143096f },
    { 0.00251046f, 0.135565f }
};
std::vector<Point> bocaB = {
    { 0, 0.0125523f },
    { -0.065272f, 0.0301255f },
    { -0.0753138f, 0.0677824f },
    { -0.00251046f, 0.065272f },
    { 0, 0.065272f }
};

Figure circle = genCircle();
Figure cuerpo = genBezier(cuerpoB);
Figure cara = genBezier(caraB);
Figure cabello1 = genBezier(cabello1B);
Figure cabello2 = genBezier(cabello2B);
Figure cabello3 = genBezier(cabello3B);
Figure cabello4 = genBezier(cabello4B);
Figure corona = genBezier(coronaB);
Figure coronaint = genBezier(coronaintB);
Figure brazo1 = genBezier(brazo1B);
Figure brazo2 = genBezier(brazo2B);
Figure brazo3 = genBezier(brazo3B);
Figure brazo4 = genBezier(brazo4B);
Figure brazo5 = genBezier(brazo5B);
Figure brazo6 = genBezier(brazo6B);
Figure mano = genBezier(manoB);
Figure ojo = genBezier(ojoB);
Figure nariz = genBezier(narizB);
Figure boca = genBezier(bocaB);

void drawMitad()
{
    // Cuerpo
    draw(BORDER, cuerpo);
    draw(BORDER, cara);

    // Cabello
    draw(BORDER, cabello1);
    draw(BORDER, cabello2);
    draw(BORDER, cabello3);
    draw(BORDER, cabello4);
    // Corona
    draw(BORDER, corona);
    draw(BORDER, coronaint);
    // Brazos
    draw(BORDER, brazo1);
    draw(BORDER, brazo2);
    draw(BORDER, brazo3);
    draw(BORDER, brazo4);
    draw(BORDER, brazo5);
    draw(BORDER, brazo6);
    // Manos
    draw(BORDER, mano);
    // Detalles
    draw(BORDER, ojo);
    draw(BORDER, nariz);
    draw(BORDER, boca);
}

void drawShape()
{
    drawWithScale(BORDER, circle, r0, r0);
    drawMitad();
    glPushMatrix();
    glScalef(-1, 1, 1);
    drawMitad();
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
    glutCreateWindow("Starbucks");

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
