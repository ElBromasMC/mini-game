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

void drawSector(
    float cx, float cy, 
    float radius,
    float t1, float t2,
    float *RGB1, float *RGB2,
    float w = 1.0f,
    int segments=100) {
  
    glColor3fv(RGB1);   

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < segments; i++) {
        float theta = t1 + i * (t2 - t1) / segments;
        float theta2 = t1 + (i + 1) * (t2 - t1) / segments;
        float x1 = cx + radius * cosf(theta);
        float y1 = cy + radius * sinf(theta);
        float x2 = cx + radius * cosf(theta2);
        float y2 = cy + radius * sinf(theta2);
        glVertex2f(cx, cy);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    }
    glEnd(); 

    glColor3fv(RGB2);   
    glLineWidth(w);

    glBegin(GL_LINE_LOOP);

    for (int i = 0; i < segments; i++) {
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
}




void display(void) {
    glClearColor(1, 1, 1, 1);  
    glClear(GL_COLOR_BUFFER_BIT);


    for(int i =0; i<15;i++){
        float r = 4, theta = 2*PI/15;
        float x = r*cosf(i*theta);
        float y = r*sinf(i*theta);
        drawSector(x,y,0.95,0,2*PI,blanco,negro);
    }
    drawSector(0,0,4.35,0,2*PI,blanco,blanco);

     for(int i =0; i<15;i++){
        float r = 3.75, theta = 2*PI/15;
        float x = r*cosf(i*theta);
        float y = r*sinf(i*theta);
        drawSector(x,y,0.85,0,2*PI,blanco,negro);
    }
    drawSector(0,0,3.75,0,2*PI,blanco,blanco);

    drawSector(0,0,3.5,0,2*PI,negro,negro);


    for(int i =0; i<15;i++){
        float r = 2.8, theta = 2*PI/15;
        float x = r*cosf(i*theta);
        float y = r*sinf(i*theta);
        drawSector(x,y,0.65,0,2*PI,blanco,negro);
    }
    drawSector(0,0,3,0,2*PI,blanco,blanco);

    for(int i =0; i<15;i++){
        float r = 2.5, theta = 2*PI/15;
        float x = r*cosf(i*theta);
        float y = r*sinf(i*theta);
        drawSector(x,y,0.65,0,2*PI,blanco,negro);
    }
    drawSector(0,0,2.8,0,2*PI,blanco,blanco);


    for(int i =0; i<15;i++){
        float r = 2.5, theta = 2*PI/15;
        float x = r*cosf(i*theta);
        float y = r*sinf(i*theta);
        drawSector(x,y,0.1,0,2*PI,blanco,negro);
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


    glutCreateWindow("Pregunta 3");
    inicio();
    glutDisplayFunc(display);
    glutMainLoop();

    return EXIT_SUCCESS; 
}