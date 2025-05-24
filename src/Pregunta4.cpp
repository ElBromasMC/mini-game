// Nombre: Diego A. Espinoza

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <stdlib.h>
#include <math.h>
#include <cstdlib>
#define PI 3.141592653589793f




float amarillo[3]     = {1, 1, 0},
      rojo[3]         = {1, 0, 0}, 
      verde[3]        = {0, 1, 0}, 
      azul[3]         = {0, 0, 1},
      cyan[3]         = {0, 1, 1},
      magenta[3]      = {1, 0, 1},
      blanco[3]       = {1, 1, 1},
      gris[3]         = {0.5f, 0.5f, 0.5f},
      naranja[3]      = {1, 0.647f, 0},
      rosa[3]         = {1, 0.75f, 0.8f},
      marron[3]       = {0.6f, 0.3f, 0},
      verde_oscuro[3] = {0, 0.5f, 0},
      azul_oscuro[3]  = {0, 0, 0.5f},
      aqua[3]         = {0, 1, 0.5f},
      crema[3]        = {1, 0.9, 0.8},
      negro[3]        = {0,0,0},
      celeste[3] = {0.5, 1, 1}; 

void drawTangentsToCircle(
        float cx, float cy,
        float radius,
        float x2, float y2, float *RGB
    ) {
        float dx = x2 - cx;
        float dy = y2 - cy;
        float distSq = dx * dx + dy * dy;
        float dist = sqrtf(distSq);
    
        if (dist <= radius) return; // No hay tangentes reales
    
        // Ángulo entre el vector (cx, cy) -> (x2, y2)
        float angleToPoint = atan2f(dy, dx);
    
        // Ángulo de desviación para las tangentes
        float offset = acosf(radius / dist);
    
        // Primer punto de tangencia
        float theta1 = angleToPoint + offset;
        float tx1 = cx + radius * cosf(theta1);
        float ty1 = cy + radius * sinf(theta1);
    
        // Segundo punto de tangencia
        float theta2 = angleToPoint - offset;
        float tx2 = cx + radius * cosf(theta2);
        float ty2 = cy + radius * sinf(theta2);
        
        glColor3fv(RGB); // rojo
        glBegin(GL_POLYGON);
            glVertex2f(x2, y2);
            glVertex2f(tx1, ty1);
            glVertex2f(tx2, ty2);
        glEnd();

        // Dibuja las líneas tangentes
        glColor3fv(RGB); 
        glBegin(GL_LINE_LOOP);
            glVertex2f(x2, y2);
            glVertex2f(tx1, ty1);
            glVertex2f(x2, y2);
            glVertex2f(tx2, ty2);
        glEnd();
    }

void drawSector(
        float cx, float cy,
        float tx, float ty,
        float radius,
        float t1, float t2,
        float *RGB, bool op) {
        
        glColor3fv(RGB);
        int segments = 300;
        
        glBegin(GL_POLYGON);
        //glVertex2f(cx, cy);
        for (int i = 0; i < segments; ++i) {
            float theta = t1 + i * (t2 - t1) / segments;
            float theta2 = t1 + (i + 1) * (t2 - t1) / segments;
            float x1 = cx + radius * cosf(theta);
            float y1 = cy + radius * sinf(theta);
            float x2 = cx + radius * cosf(theta2);
            float y2 = cy + radius * sinf(theta2);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
        }
        glEnd();
        if (op)
        {
            drawTangentsToCircle(cx, cy, radius, tx, ty,RGB);
        }
    }


void display(void) {
    glClearColor(1, 1, 1, 1);  
    glClear(GL_COLOR_BUFFER_BIT);
 drawSector(0, 0,0,0, 4.77,0, 2*PI,negro,false);
 drawSector(0, 0,0,0, 4.75,0, 2*PI,celeste,false);

for (int i = 0; i < 19; i++)
{
        float r = 4, theta = 2*PI/19;
        float x = r*cosf(i*theta + PI/19);
        float y = r*sinf(i*theta+ PI/19);
        drawSector(x, y,0,0, 0.5,0, 2*PI,negro,false);
        drawSector(x, y,0,0, 0.48,0, 2*PI,amarillo,false);
        drawSector(x, y,0,0, 0.42,0, 2*PI,rojo,false);
        drawSector(x, y,0,0, 0.36,0, 2*PI,celeste,false);

}



 drawSector(0, 0,0,0, 4,0, 2*PI,negro,false);
drawSector(0, 0,0,0, 3.9,0, 2*PI,rojo,false);

for (int i = 0; i < 4; i++)
{
        float r = 3, theta = 2*PI/4;
        float x = r*cosf(i*theta + PI/4);
        float y = r*sinf(i*theta+ PI/4);
        drawSector(x, y,0,0, 0.85,0, 2*PI,negro,false);
        drawSector(x, y,0,0, 0.8,0, 2*PI,amarillo,false);
                drawSector(x, y,0,0, 0.77,0, 2*PI,negro,false);

        drawSector(x, y,0,0, 0.75,0, 2*PI,blanco,false);
                        drawSector(x, y,0,0, 0.7,0, 2*PI,negro,false);

        drawSector(x, y,0,0, 0.67,0, 2*PI,celeste,false);

}
   







drawSector(0, 0,0,0, 3,0, 2*PI,negro,false);
drawSector(0, 0,0,0, 2.9,0, 2*PI,blanco,false);

for (int i = 0; i < 6; i++)
{
        float r = 2.25, theta = 2*PI/6;
        float x = r*cosf(i*theta + PI/6);
        float y = r*sinf(i*theta+ PI/6);
        drawSector(x, y,0,0, 0.6,0, 2*PI,celeste,false);
        drawSector(x, y,0,0, 0.5,0, 2*PI,rojo,false);
        drawSector(x, y,0,0, 0.4,0, 2*PI,amarillo,false);

}


drawSector(0, 0,0,0, 2.3,0, 2*PI,negro,false);
drawSector(0, 0,0,0, 2.2,0, 2*PI,celeste,false);
drawSector(0, 0,0,0, 2,0, 2*PI,negro,false);
drawSector(0, 0,0,0, 1.9,0, 2*PI,amarillo,false);


// Aros arriba
drawSector(0, 1,0,0, 0.78,0, 2*PI,negro, true);
drawSector(0, 1,0,0, 0.72,0, 2*PI,rojo, true);

drawSector(0, 1,0,0, 0.58,0, 2*PI,negro, true);
drawSector(0, 1,0,0, 0.52,0, 2*PI,blanco, true);

drawSector(0, 1,0,0, 0.38,0, 2*PI,negro, true);
drawSector(0, 1,0,0, 0.32,0, 2*PI,amarillo, true);

// Aros Bajo
drawSector(0, -1,0,0, 0.78,0, 2*PI,negro, true);
drawSector(0, -1,0,0, 0.72,0, 2*PI,rojo, true);

drawSector(0, -1,0,0, 0.58,0, 2*PI,negro, true);
drawSector(0, -1,0,0, 0.52,0, 2*PI,blanco, true);

drawSector(0, -1,0,0, 0.38,0, 2*PI,negro, true);
drawSector(0, -1,0,0, 0.32,0, 2*PI,amarillo, true);

// Circulo
drawSector(0, 0,0,0, 0.75,0, 2*PI,negro, false);
drawSector(0, 0,0,0, 0.65,0, 2*PI,blanco, false);
drawSector(0, 0,0,0, 0.55,0, 2*PI,rojo, false);


    glutSwapBuffers();  
}


void inicio()
{   
    int a = 5;
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-a,a,-a,a);
    glClearColor(0,0,0,0);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv); 
    glutInitWindowSize(800,800);
    glutInitWindowPosition(500,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);


    glutCreateWindow("Pregunta 4");
    inicio();
    glutDisplayFunc(display);
    glutMainLoop();

    return EXIT_SUCCESS; 
}