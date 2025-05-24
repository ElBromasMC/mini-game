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




float blanco[3]       = {1, 1, 1},
      negro[3]        = {0,0,0};


void polygon(float x, float y, float r, float n,float *RGB, GLenum MODO){

    glColor3fv(RGB);
    glBegin(MODO);
        float angulo = 2 * M_PI / n;
        for (int i = 0; i < n; i++) {
            float vx = x + r * cos(i * angulo);
            float vy = y + r * sin(i * angulo);
            glVertex2f(vx, vy);
        }
    glEnd();
}


void elipse(
        float cx, float cy,
        float rx, float ry,
        float t1, float t2,
        float *RGB2,
        float w=1.0f)
    {
        float segments = 100.0f; 

        glColor3fv(RGB2);   
        glLineWidth(w);

        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; i++) {
    
            float theta = t1 + (t2 - t1)* float(i) / float(segments);
            float theta2 = t1 + (i + 1) * (t2 - t1) / segments;
            float x1 = cx + rx * cosf(theta);
            float y1 = cy + ry * sinf(theta);
            float x2 = cx + rx * cosf(theta2);
            float y2 = cy + ry * sinf(theta2);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
        }
        glEnd(); 
    }



void display(void) {
    glClearColor(1, 1, 1, 1);  
    glClear(GL_COLOR_BUFFER_BIT);

    polygon(0,0,4.75,100,negro,GL_LINE_LOOP);
    polygon(0,0,4.5,100,negro,GL_LINE_LOOP);

    for (int i = 0; i < 8; i++)
    {
        float r = 4, theta = PI/4;
        float x = r*cosf(i*theta + PI/6);
        float y = r*sinf(i*theta+ PI/6);
        polygon(x,y,0.15,100,negro,GL_LINE_LOOP);

    }

    for (int i = 0; i < 8; i++)
    {
        float r = 3.5, theta = PI/4;
        float x = r*cosf(i*theta );
        float y = r*sinf(i*theta);
        polygon(x,y,0.25,100,negro,GL_LINE_LOOP);

    }
    
    for (int i = 0; i < 8; i++)
    {
        float r = 2.5, theta = PI/4;
        float x = r*cosf(i*theta );
        float y = r*sinf(i*theta);
        polygon(x,y,0.25,100,negro,GL_LINE_LOOP);
        polygon(x,y,0.5,100,negro,GL_LINE_LOOP);

    }

    for (int i = 0; i < 8; i++)
    {
        float r = 1.5, theta = PI/4;
        float x = r*cosf(i*theta );
        float y = r*sinf(i*theta);
        polygon(x,y,0.15,100,negro,GL_LINE_LOOP);

    }
    

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


    glutCreateWindow("Pregunta 2");
    inicio();
    glutDisplayFunc(display);
    glutMainLoop();

    return EXIT_SUCCESS; 
}