#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iomanip>
#include <sstream>
#include <string>

bool debug = true;
GLuint textureID = 0;
int imageWidth = 0;
int imageHeight = 0;
bool g_ShowMouseCoords = false;
double g_MouseWorldX = 0.0;
double g_MouseWorldY = 0.0;

bool loadTexture(const char* filename)
{
    int channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &imageWidth, &imageHeight, &channels, 0);

    if (!data) {
        std::cerr << "ERROR: Could not load texture file '" << filename << "'"
                  << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }

    GLenum format;
    if (channels == 1) {
        format = GL_LUMINANCE;
        std::cout << "Loaded texture: " << filename << " (" << imageWidth << "x"
                  << imageHeight << "), Grayscale" << std::endl;
    } else if (channels == 3) {
        format = GL_RGB; // RGB
        std::cout << "Loaded texture: " << filename << " (" << imageWidth << "x"
                  << imageHeight << "), RGB" << std::endl;
    } else if (channels == 4) {
        format = GL_RGBA; // RGBA
        std::cout << "Loaded texture: " << filename << " (" << imageWidth << "x"
                  << imageHeight << "), RGBA" << std::endl;
    } else {
        // Should not happen with standard formats, but good to check
        std::cerr << "ERROR: Unsupported number of channels (" << channels
                  << ") in image '" << filename << "'" << std::endl;
        stbi_image_free(data); // Free the allocated memory
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format,
        GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void drawCoordinatesText()
{
    if (!g_ShowMouseCoords) {
        return;
    }

    float radiusMouse = sqrtf(g_MouseWorldX * g_MouseWorldX + g_MouseWorldY * g_MouseWorldY);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << "Clicked: X=" << g_MouseWorldX << ", Y=" << g_MouseWorldY
       << ", r=" << radiusMouse;
    std::string coordString = ss.str();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluOrtho2D(0, viewport[2], 0, viewport[3]);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0f, 0.0f, 0.0f);
    float xPos = 10.0f;
    float yPos = 10.0f;
    glRasterPos2f(xPos, yPos);
    void* font = GLUT_BITMAP_HELVETICA_12;
    for (char c : coordString) {
        glutBitmapCharacter(font, c);
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void drawImageReference()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);

    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, -1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void drawAxes(float extent, float lineWidth)
{
    glLineWidth(lineWidth);
    glColor4f(0, 0, 1, 0.3);
    glBegin(GL_LINES);
    glVertex2f(-extent, 0.0f);
    glVertex2f(extent, 0.0f);
    glVertex2f(0.0f, -extent);
    glVertex2f(0.0f, extent);
    glEnd();
    glLineWidth(1.0f);
}

void mouseCallback(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLdouble modelview[16];
        GLdouble projection[16];
        GLint viewport[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);

        GLfloat windowY = (GLfloat)(viewport[3] - y);
        GLfloat windowX = (GLfloat)x;

        GLfloat windowZ = 0.0f;

        GLdouble worldX, worldY, worldZ;
        gluUnProject(windowX, windowY, windowZ, modelview, projection, viewport,
            &worldX, &worldY, &worldZ);

        // Store coordinates globally instead of printing
        g_MouseWorldX = worldX;
        g_MouseWorldY = worldY;
        g_ShowMouseCoords = true;

        // Request a redraw to display the updated coordinates
        glutPostRedisplay();
    }
}

void keyboardCallback(unsigned char key, int x, int y)
{
    switch (key) {
    case ' ':
        debug = !debug;
        glutPostRedisplay();
        break;

    default:
        break;
    }
}
