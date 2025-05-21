#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iomanip>
#include <sstream>
#include <string>

typedef struct {
    float x;
    float y;
} DebugPoint;

// ... (your existing globals) ...

// BEZIER MODE State
bool g_BezierMode = false;
std::vector<DebugPoint> g_CurrentBezierInteractivePoints; // Stores P0, P2, then P0, P1, P2 for the segment being defined
                                                          // Max 3 points: P0, P1 (control), P2
DebugPoint g_TempMousePoint; // For real-time feedback of potential points
bool g_PlacingControlPoint = false; // True if right mouse is down for control point
std::vector<DebugPoint> g_FinalBezierPathPoints; // Stores the sequence (coord, control, coord, control...)

bool debug = true;
GLuint textureID = 0;
int imageWidth = 0;
int imageHeight = 0;
bool g_ShowMouseCoords = false;
double g_MouseWorldX = 0.0;
double g_MouseWorldY = 0.0;

// Function to calculate a point on a quadratic Bezier curve
DebugPoint getQuadraticBezierPoint(float t, DebugPoint p0, DebugPoint p1, DebugPoint p2)
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float ut = u * t; // 2 * u * t

    DebugPoint p;
    p.x = uu * p0.x + 2 * ut * p1.x + tt * p2.x;
    p.y = uu * p0.y + 2 * ut * p1.y + tt * p2.y;
    return p;
}

// Function to draw a quadratic Bezier curve
void drawQuadraticBezier(DebugPoint p0, DebugPoint p1, DebugPoint p2, int segments = 30)
{
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float t = (float)i / (float)segments;
        DebugPoint p = getQuadraticBezierPoint(t, p0, p1, p2);
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

// Helper to draw a small marker (e.g., a square or circle)
void drawMarker(DebugPoint p, float r, float g, float b, float size = 0.015f)
{
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(p.x, p.y, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-size / 2, -size / 2);
    glVertex2f(size / 2, -size / 2);
    glVertex2f(size / 2, size / 2);
    glVertex2f(-size / 2, size / 2);
    glEnd();
    glPopMatrix();
}

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
    float angleMouse = atan2f(g_MouseWorldY, g_MouseWorldX);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << "Clicked: X=" << g_MouseWorldX << ", Y=" << g_MouseWorldY
       << ", r=" << radiusMouse << ", angle=" << angleMouse;
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

    g_MouseWorldX = worldX; // For general coordinate display
    g_MouseWorldY = worldY;
    g_ShowMouseCoords = true;

    DebugPoint currentPoint = { (float)worldX, (float)worldY };

    if (g_BezierMode) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            g_PlacingControlPoint = false; // Stop placing control point if we were

            if (g_CurrentBezierInteractivePoints.empty()) { // Expecting P0
                g_CurrentBezierInteractivePoints.push_back(currentPoint);
                std::cout << "   P0 (Start) set: (" << currentPoint.x << ", " << currentPoint.y << ")" << std::endl;
                std::cout << "2. Left-click to set END point (P2)." << std::endl;
            } else if (g_CurrentBezierInteractivePoints.size() == 1) { // Expecting P2 (P0 is already there)
                g_CurrentBezierInteractivePoints.push_back(currentPoint); // This is P2
                std::cout << "   P2 (End) set: (" << currentPoint.x << ", " << currentPoint.y << ")" << std::endl;
                std::cout << "3. Right-click & DRAG to set CONTROL point (P1), release when done." << std::endl;
            } else if (g_CurrentBezierInteractivePoints.size() == 2) { // P0, P2 are set, expecting P1 (control) via right click
                // Or user might be re-clicking P2
                g_CurrentBezierInteractivePoints.back() = currentPoint; // Update P2
                std::cout << "   P2 (End) RE-SET: (" << currentPoint.x << ", " << currentPoint.y << ")" << std::endl;
                std::cout << "3. Right-click & DRAG to set CONTROL point (P1), release when done." << std::endl;
            } else if (g_CurrentBezierInteractivePoints.size() == 3) { // P0, P1, P2 are set. User might be adjusting P0 or P2
                // This case means a segment is fully defined. A new left click could mean
                // the user wants to discard current P1 and P2 and set a new P2 for the existing P0.
                // Or, if they press ENTER, it finalizes.
                // For simplicity, let's assume they want to redefine P2 if they left-click now.
                // The P1 will be cleared, requiring a new right-click.
                DebugPoint p0 = g_CurrentBezierInteractivePoints[0];
                g_CurrentBezierInteractivePoints.clear();
                g_CurrentBezierInteractivePoints.push_back(p0);
                g_CurrentBezierInteractivePoints.push_back(currentPoint); // New P2
                std::cout << "   Segment points P1 and P2 cleared by new Left-Click." << std::endl;
                std::cout << "   P0 (Start) is: (" << p0.x << ", " << p0.y << ")" << std::endl;
                std::cout << "   P2 (End) RE-SET: (" << currentPoint.x << ", " << currentPoint.y << ")" << std::endl;
                std::cout << "3. Right-click & DRAG to set CONTROL point (P1), release when done." << std::endl;
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            if (g_CurrentBezierInteractivePoints.size() == 2) { // We have P0 and P2, ready for P1 (control)
                if (state == GLUT_DOWN) {
                    g_PlacingControlPoint = true;
                    // Insert temporary P1 between P0 and P2
                    g_CurrentBezierInteractivePoints.insert(g_CurrentBezierInteractivePoints.begin() + 1, currentPoint);
                    std::cout << "   CONTROL point (P1) placement STARTED at: (" << currentPoint.x << ", " << currentPoint.y << ")" << std::endl;
                } else if (state == GLUT_UP && g_PlacingControlPoint) {
                    g_PlacingControlPoint = false;
                    // P1 is already at g_CurrentBezierInteractivePoints[1] due to motion updates
                    std::cout << "   CONTROL point (P1) set: (" << g_CurrentBezierInteractivePoints[1].x << ", " << g_CurrentBezierInteractivePoints[1].y << ")" << std::endl;
                    std::cout << "Press ENTER to finalize segment, or adjust points." << std::endl;
                }
            } else if (g_CurrentBezierInteractivePoints.size() == 3 && state == GLUT_DOWN) { // P0, P1, P2 exist, adjusting P1
                g_PlacingControlPoint = true; // Allow re-dragging P1
                g_CurrentBezierInteractivePoints[1] = currentPoint; // Update P1 immediately
                std::cout << "   CONTROL point (P1) adjustment STARTED at: (" << currentPoint.x << ", " << currentPoint.y << ")" << std::endl;
            } else if (g_CurrentBezierInteractivePoints.size() == 3 && state == GLUT_UP && g_PlacingControlPoint) {
                g_PlacingControlPoint = false;
                std::cout << "   CONTROL point (P1) re-set: (" << g_CurrentBezierInteractivePoints[1].x << ", " << g_CurrentBezierInteractivePoints[1].y << ")" << std::endl;
                std::cout << "Press ENTER to finalize segment, or adjust points." << std::endl;
            } else {
                if (state == GLUT_DOWN) {
                    std::cout << "RIGHT-CLICK: Please set P0 and P2 (2 left-clicks) before defining control point P1." << std::endl;
                }
            }
        }
    }
    // Non-Bezier mode clicks are handled by g_ShowMouseCoords update earlier
    glutPostRedisplay();
}

void mouseMotionCallback(int x, int y)
{
    if (g_BezierMode) {
        // Convert screen coordinates to world coordinates
        GLdouble modelview[16];
        GLdouble projection[16];
        GLint viewport[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);
        GLfloat windowY = (GLfloat)(viewport[3] - y);
        GLfloat windowX = (GLfloat)x;
        GLfloat windowZ = 0.0f;
        GLdouble worldX_double, worldY_double, worldZ_double;
        gluUnProject(windowX, windowY, windowZ, modelview, projection, viewport,
            &worldX_double, &worldY_double, &worldZ_double);

        DebugPoint motionPoint = { (float)worldX_double, (float)worldY_double };
        g_TempMousePoint = motionPoint; // Store for potential drawing if needed

        // Update general mouse coordinate display
        g_MouseWorldX = worldX_double;
        g_MouseWorldY = worldY_double;
        // g_ShowMouseCoords = true; // (optional: show coords on motion too)

        if (g_BezierMode && g_PlacingControlPoint && g_CurrentBezierInteractivePoints.size() == 3) {
            // We are dragging the right mouse button to place/adjust P1 (control point)
            // P0 is at index 0, P1 (control) is at index 1, P2 is at index 2
            g_CurrentBezierInteractivePoints[1] = motionPoint;
            glutPostRedisplay();
        } else {
            // If just moving the mouse (no buttons pressed, or not in a relevant Bezier state)
            // and you want to see where a point *would* be placed.
            // This is good for passive feedback before a click.
            glutPostRedisplay(); // Ensure coords text updates if shown
        }
    }
}

void keyboardCallback(unsigned char key, int x, int y)
{
    switch (key) {
    case ' ':
        debug = !debug;
        if (debug) {
            std::cout << "Debug mode ON" << std::endl;
        } else {
            std::cout << "Debug mode OFF" << std::endl;
        }
        glutPostRedisplay();
        break;

    case 'b': // Toggle Bezier Mode
    case 'B':
        g_BezierMode = !g_BezierMode;
        g_CurrentBezierInteractivePoints.clear();
        g_PlacingControlPoint = false;
        if (g_BezierMode) {
            std::cout << "-----------------------------------------------------" << std::endl;
            std::cout << "BEZIER MODE ACTIVATED" << std::endl;
            std::cout << "1. Left-click to set START point (P0)." << std::endl;
            if (!g_FinalBezierPathPoints.empty()) {
                // Automatically use the last point of the previous segment as the new P0
                DebugPoint lastPoint = g_FinalBezierPathPoints.back();
                g_CurrentBezierInteractivePoints.push_back(lastPoint);
                std::cout << "   Continuing from last point: P0=(" << lastPoint.x << ", " << lastPoint.y << ")" << std::endl;
                std::cout << "2. Left-click to set END point (P2)." << std::endl;
            } else {
                std::cout << "1. Left-click to set START point (P0)." << std::endl;
            }
        } else {
            std::cout << "BEZIER MODE DEACTIVATED" << std::endl;
            std::cout << "-----------------------------------------------------" << std::endl;
            // Output the collected points when exiting Bezier Mode
            if (!g_FinalBezierPathPoints.empty()) {
                std::cout << "Collected Bezier Path Points (copy this into your vector):" << std::endl;
                std::cout << "std::vector<Point> segmentPoints = {" << std::endl;
                for (size_t i = 0; i < g_FinalBezierPathPoints.size(); ++i) {
                    std::cout << "    { " << g_FinalBezierPathPoints[i].x << "f, " << g_FinalBezierPathPoints[i].y << "f }";
                    if (i < g_FinalBezierPathPoints.size() - 1) {
                        std::cout << ",";
                    }
                    std::cout << std::endl;
                }
                std::cout << "};" << std::endl;
            }
        }
        glutPostRedisplay();
        break;

    case 13: // ASCII for Enter key
        if (g_BezierMode && g_CurrentBezierInteractivePoints.size() == 3) {
            // We have P0, P1 (control), P2. Finalize this segment.
            DebugPoint p0 = g_CurrentBezierInteractivePoints[0];
            DebugPoint p1 = g_CurrentBezierInteractivePoints[1]; // Control Point
            DebugPoint p2 = g_CurrentBezierInteractivePoints[2];

            if (g_FinalBezierPathPoints.empty() || (g_FinalBezierPathPoints.back().x != p0.x || g_FinalBezierPathPoints.back().y != p0.y)) {
                // If it's the very first point or P0 is not the last point of the path
                g_FinalBezierPathPoints.push_back(p0);
            }
            g_FinalBezierPathPoints.push_back(p1);
            g_FinalBezierPathPoints.push_back(p2);

            std::cout << "Segment Added: P0(" << p0.x << "," << p0.y << ") - P1_ctrl(" << p1.x << "," << p1.y << ") - P2(" << p2.x << "," << p2.y << ")" << std::endl;

            // Prepare for the next segment: P2 becomes the new P0
            g_CurrentBezierInteractivePoints.clear();
            g_CurrentBezierInteractivePoints.push_back(p2); // New P0
            g_PlacingControlPoint = false;

            std::cout << "   Next segment: P0 automatically set to (" << p2.x << ", " << p2.y << ")." << std::endl;
            std::cout << "2. Left-click to set END point (P2) for the new segment." << std::endl;

        } else if (g_BezierMode) {
            std::cout << "ENTER: Cannot finalize segment. Need P0, P1 (control), and P2." << std::endl;
            std::cout << "   Current points: " << g_CurrentBezierInteractivePoints.size() << std::endl;
            std::cout << "   Ensure you've set Start (Left-Click), End (Left-Click), and Control (Right-Click Drag & Release)." << std::endl;
        }
        glutPostRedisplay();
        break;

    case 'c': // Clear current Bezier segment and optionally all points
    case 'C':
        if (g_BezierMode) {
            std::cout << "CLEAR: Current Bezier segment points cleared." << std::endl;
            g_CurrentBezierInteractivePoints.clear();
            g_PlacingControlPoint = false;
            // To clear the entire path as well, you might add a SHIFT+C or another key
            // g_FinalBezierPathPoints.clear();
            // std::cout << "CLEAR: Entire Bezier path cleared." << std::endl;
            if (!g_FinalBezierPathPoints.empty()) {
                DebugPoint lastPoint = g_FinalBezierPathPoints.back();
                g_CurrentBezierInteractivePoints.push_back(lastPoint);
                std::cout << "   Restarting segment from last finalized point: P0=(" << lastPoint.x << ", " << lastPoint.y << ")" << std::endl;
                std::cout << "2. Left-click to set END point (P2)." << std::endl;
            } else {
                std::cout << "1. Left-click to set START point (P0)." << std::endl;
            }
        }
        glutPostRedisplay();
        break;

    default:
        break;
    }
}

void drawBezierDebug()
{
    // --- BEZIER MODE DRAWING LOGIC ---
    if (g_BezierMode) {
        // Draw finalized path segments
        if (g_FinalBezierPathPoints.size() >= 3) {
            glColor3f(0.4f, 0.4f, 1.0f); // Color for finalized path
            glLineWidth(1.5f);
            for (size_t i = 0; (i + 2) < g_FinalBezierPathPoints.size(); i += 2) {
                // Path is P0, C1, P1, C2, P2 ...
                // A segment is P_i, C_i+1, P_i+1 if P_i is an anchor.
                // More directly: P0, P1_ctrl, P2. So every 3 points in g_FinalBezierPathPoints.
                // However, the structure is (coord, control, coord, control, coord...)
                // So g_FinalBezierPathPoints[i] is P_start, g_FinalBezierPathPoints[i+1] is P_control, g_FinalBezierPathPoints[i+2] is P_end
                if ((i % 2 == 0) && (i + 2 < g_FinalBezierPathPoints.size())) { // Ensure we have a full P0, C, P2 triplet
                    DebugPoint segP0 = g_FinalBezierPathPoints[i];
                    DebugPoint segP1_ctrl = g_FinalBezierPathPoints[i + 1];
                    DebugPoint segP2 = g_FinalBezierPathPoints[i + 2];
                    drawQuadraticBezier(segP0, segP1_ctrl, segP2);
                    // Draw markers for finalized segment points
                    drawMarker(segP0, 0.3f, 0.3f, 0.8f, 0.01f); // Start
                    drawMarker(segP1_ctrl, 0.3f, 0.8f, 0.3f, 0.01f); // Control
                    drawMarker(segP2, 0.3f, 0.3f, 0.8f, 0.01f); // End
                }
            }
            glLineWidth(1.0f);
        }

        // Draw current interactive points and curve
        if (!g_CurrentBezierInteractivePoints.empty()) {
            DebugPoint p0 = g_CurrentBezierInteractivePoints[0];
            drawMarker(p0, 1.0f, 0.0f, 0.0f); // P0 in Red

            if (g_CurrentBezierInteractivePoints.size() >= 2) {
                DebugPoint p_potential_p2_or_p1; // could be P2 or P1 depending on size
                if (g_CurrentBezierInteractivePoints.size() == 2) { // P0, P2 are set (P2 is at index 1 here)
                    p_potential_p2_or_p1 = g_CurrentBezierInteractivePoints[1];
                    drawMarker(p_potential_p2_or_p1, 0.0f, 0.0f, 1.0f); // P2 in Blue
                                                                        // Draw a line from P0 to P2 if only these two are set
                    glColor3f(0.7f, 0.7f, 0.7f); // Light grey line
                    glBegin(GL_LINES);
                    glVertex2f(p0.x, p0.y);
                    glVertex2f(p_potential_p2_or_p1.x, p_potential_p2_or_p1.y);
                    glEnd();

                    // Show potential control point placement if hovering with right mouse (not yet clicked)
                    // This needs g_TempMousePoint to be updated by passive motion
                    if (!g_PlacingControlPoint) { // Only if not actively dragging
                        drawMarker(g_TempMousePoint, 0.0f, 0.8f, 0.0f, 0.01f); // Temporary Control Point in light green
                        glColor3f(0.0f, 0.8f, 0.0f); // Preview curve color (e.g., light green)
                        glEnable(GL_LINE_STIPPLE);
                        glLineStipple(1, 0xAAAA); // Dashed line
                        drawQuadraticBezier(p0, g_TempMousePoint, p_potential_p2_or_p1);
                        glDisable(GL_LINE_STIPPLE);
                    }

                } else if (g_CurrentBezierInteractivePoints.size() == 3) { // P0, P1, P2 are set
                    DebugPoint p1_control = g_CurrentBezierInteractivePoints[1];
                    DebugPoint p2_end = g_CurrentBezierInteractivePoints[2];

                    drawMarker(p1_control, 0.0f, 1.0f, 0.0f); // P1 (Control) in Green
                    drawMarker(p2_end, 0.0f, 0.0f, 1.0f); // P2 (End) in Blue

                    // Draw the interactive Bezier curve
                    glColor3f(0.0f, 0.6f, 0.0f); // Active Bezier curve color (e.g., darker green)
                    glLineWidth(2.0f);
                    drawQuadraticBezier(p0, p1_control, p2_end);
                    glLineWidth(1.0f);

                    // Draw lines from P0-P1 and P1-P2 (control polygon)
                    glColor3f(0.5f, 0.5f, 0.5f); // Grey for control lines
                    glBegin(GL_LINE_STRIP);
                    glVertex2f(p0.x, p0.y);
                    glVertex2f(p1_control.x, p1_control.y);
                    glVertex2f(p2_end.x, p2_end.y);
                    glEnd();
                }
            } else { // Only P0 is set, show temp mouse point for P2
                drawMarker(g_TempMousePoint, 0.0f, 0.0f, 0.8f, 0.01f); // Light blue for potential P2
                glColor3f(0.7f, 0.7f, 0.7f);
                glBegin(GL_LINES);
                glVertex2f(p0.x, p0.y);
                glVertex2f(g_TempMousePoint.x, g_TempMousePoint.y);
                glEnd();
            }
        } else if (g_BezierMode) { // Bezier mode is on, but no points placed yet
            drawMarker(g_TempMousePoint, 0.8f, 0.0f, 0.0f, 0.01f); // Light red for potential P0
        }
    }
}
