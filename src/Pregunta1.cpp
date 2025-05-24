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


void drawPolygon(
    float x, float y,                       
    float radiusX, float radiusY,                                   
    float *RGB1, float *RGB2, 
    int segments = 300,
    int op =0,
    float pivotX = 0, float pivotY = 0,               
    float orbitAngleRad = 0) {

    glColor3fv(RGB1);
    glBegin(GL_POLYGON);
    float cosOrbit = cosf(orbitAngleRad);
    float sinOrbit = sinf(orbitAngleRad);

    for (int i = 0; i < segments; ++i) {
        float base_theta = ((float)i / (float)segments) * (2.0f * PI);
        
        float cx = radiusX * cosf(base_theta);
        float cy = radiusY * sinf(base_theta);
        
        float relativePivotX = (x + cx) - pivotX;
        float relativePivotY = (y + cy) - pivotY;
        
        float x_rotated = relativePivotX * cosOrbit - relativePivotY * sinOrbit;
        float y_rotated = relativePivotX * sinOrbit + relativePivotY * cosOrbit;
        
        
        glVertex2f(x_rotated + pivotX, y_rotated + pivotY);
        if(op!=0){
             glVertex2f(x, y);
            }
    }
    glEnd();

    glColor3fv(RGB2);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float base_theta = ((float)i / (float)segments) * (2.0f * PI);
        
        float cx = radiusX * cosf(base_theta);
        float cy = radiusY * sinf(base_theta);
        
        float relativePivotX = (x + cx) - pivotX;
        float relativePivotY = (y + cy) - pivotY;
        
        float x_rotated = relativePivotX * cosOrbit - relativePivotY * sinOrbit;
        float y_rotated = relativePivotX * sinOrbit + relativePivotY * cosOrbit;
        
        
        glVertex2f(x_rotated + pivotX, y_rotated + pivotY);
        if(op!=0){
             glVertex2f(x, y);
            }
    }


    glEnd();
}

void polygon(float x, float y, float r, float n,float *RGB, GLenum modo){

    glColor3fv(RGB);
    glBegin(modo);
        float angulo = 2 * M_PI / n;
        for (int i = 0; i < n; i++) {
            float vx = x + r * cos(i * angulo);
            float vy = y + r * sin(i * angulo);
            glVertex2f(vx, vy);
        }
    glEnd();
}

void circle(
    float x, float y,
    float r, float n,
    float angle1, float angle2,
    float *RGB){
    glColor3fv(RGB);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < n; i++)
        {
            float startAngle = angle1 + i*(angle2-angle1)/n;
            float x1 = x + r*cosf(startAngle);
            float y1 = y + r*sinf(startAngle);
           
            float endAngle = angle1 + (i+1)*(angle2-angle1)/n;
            float x2 = x + r*cosf(endAngle);
            float y2 = y + r*sinf(endAngle);
            
            glVertex2f(x1,y1);
            glVertex2f(x,y);
            glVertex2f(x2,y2);
        }
        
    glEnd(); 
}

void drawRingSector(
    float cx, float cy, 
    float radius_inner, float radius_outer,
    float t1, float t2,
    float *RGB1,
    int segments = 300) 
{
    glColor3fv(RGB1);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float theta = t1 + i * (t2 - t1) / segments;
        
        float x_outer = cx + radius_outer * cosf(theta);
        float y_outer = cy + radius_outer * sinf(theta);
        
        float x_inner = cx + radius_inner * cosf(theta);
        float y_inner = cy + radius_inner * sinf(theta);
        
        glVertex2f(x_outer, y_outer);
        glVertex2f(x_inner, y_inner);
    }
    glEnd();
}



void display(void) {
    glClearColor(1, 1, 1, 1);  
    glClear(GL_COLOR_BUFFER_BIT);

   
    drawPolygon(0,0,6,6,negro,negro,4,0,0,0,PI/4);
    drawPolygon(0,0,5.65,5.65,blanco,negro,4,0,0,0,PI/4);

circle(4,-4,4,50,PI/2,PI,negro);
circle(-4,-4,4,50,0,PI/2,negro);
circle(-4,4,4,50,-PI/2,0,negro);
circle(4,4,4,50,PI,1.5*PI,negro);



for (int i = 0; i < 5; i++)
    {
        float r1 = 4, sep = 0.5;
        float r = r1-i*sep;

        polygon(0,0,r,4,blanco,GL_POLYGON);
    } 

circle(0,0,2.5,100,0,2*PI,negro);
drawRingSector(0,0,2,3.5,0,2*PI,blanco,4);
for (int i = 0; i < 5; i++)
    {
        float r1 = 4, sep = 0.5;
        float r = r1-i*sep;

        polygon(0,0,r,4,negro,GL_LINE_LOOP);
    } 



for(int i =0; i<5; i++){
    drawPolygon(2,2, 1-i*0.25, 2.85,blanco,negro,4,0,2,2,-PI/4);
    drawPolygon(2,-2, 1-i*0.25, 2.85,blanco,negro,4,0,2,-2,PI/4);
    drawPolygon(-2,-2, 1-i*0.25, 2.85,blanco,negro,4,0,-2,-2,-PI/4);
    drawPolygon(-2,2, 1-i*0.25, 2.85,blanco,negro,4,0,-2,2,PI/4);

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


    glutCreateWindow("Pregunta 1");
    inicio();
    glutDisplayFunc(display);
    glutMainLoop();

    return EXIT_SUCCESS; 
}