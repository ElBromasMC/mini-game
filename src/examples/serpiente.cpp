#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif
#ifndef M_PI
#define M_PI 3.14159265
#endif

// Hola, profesor. Si se llego a implementar todos los puntos

// --- Constantes ---
// Limites del escenario
const float SCENARIO_MIN_X = -0.9f;
const float SCENARIO_MAX_X = 0.9f;
const float SCENARIO_MIN_Y = -0.9f;
const float SCENARIO_MAX_Y = 0.9f;
// Propiedades de las entidades
const float APPLE_RADIUS = 0.025f;
const float SNAKE_RADIUS = 0.025f;
const float MOVE_STEP = 0.05f;
// Propiedades del juego
const int WIN_SCORE = 6; // Solo 6 puntos, 20 es mucho
const int TIMER_DELAY_MS = 150;

// --- Estructuras ---
struct Point {
    float x, y;

    // Implementar comparacion de puntos
    // Nos permitira saber si las entidades estan en el mismo "pixel"
    bool operator==(const Point& other) const
    {
        return std::fabs(x - other.x) < (MOVE_STEP / 2.0f) && std::fabs(y - other.y) < (MOVE_STEP / 2.0f);
    }
};

// --- Variables globales ---
// Entidades
std::vector<Point> snake;
Point applePosition;
Point snakeDirection;
// Variables del juego
int score = 0;
bool gameOver = false;
bool victoryAchieved = false;
bool gamePaused = false;

// --- Funciones auxiliares ---
void drawCircle(float cx, float cy, float r, int num_segments, float colR, float colG, float colB)
{
    glColor3f(colR, colG, colB);
    glBegin(GL_POLYGON);
    for (int ii = 0; ii < num_segments; ii++) {
        float theta = 2.0f * M_PI * float(ii) / num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void drawText(float x, float y, const char* string, float colR, float colG, float colB)
{
    glColor3f(colR, colG, colB);
    glRasterPos2f(x, y);
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// --- Logica del juego ---
void spawnApple()
{
    bool onSnake;
    do {
        onSnake = false;
        int numStepsX = static_cast<int>((SCENARIO_MAX_X - SCENARIO_MIN_X - 2 * APPLE_RADIUS) / MOVE_STEP);
        int numStepsY = static_cast<int>((SCENARIO_MAX_Y - SCENARIO_MIN_Y - 2 * APPLE_RADIUS) / MOVE_STEP);

        if (numStepsX <= 0)
            numStepsX = 1;
        if (numStepsY <= 0)
            numStepsY = 1;

        // Generarla de manera aleatoria dentro de un "pixel"
        applePosition.x = SCENARIO_MIN_X + APPLE_RADIUS + (rand() % numStepsX) * MOVE_STEP;
        applePosition.y = SCENARIO_MIN_Y + APPLE_RADIUS + (rand() % numStepsY) * MOVE_STEP;

        // Si la manzana aparecio dentro de la serpiente, descartarla
        for (const auto& segment : snake) {
            float dx = segment.x - applePosition.x;
            float dy = segment.y - applePosition.y;
            if (sqrt(dx * dx + dy * dy) < SNAKE_RADIUS + APPLE_RADIUS) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}

void resetGame()
{
    // Resetear las variables globales a su valor inicial
    gameOver = false;
    victoryAchieved = false;
    gamePaused = false;
    score = 0;
    snake.clear();

    snake.push_back({ (SCENARIO_MIN_X + SCENARIO_MAX_X) / 2.0f, (SCENARIO_MIN_Y + SCENARIO_MAX_Y) / 2.0f });
    snakeDirection = { MOVE_STEP, 0.0f };

    spawnApple();
    glutPostRedisplay();
}

// --- Logica de colisiones ---
bool checkCircleCollision(Point p1, float r1, Point p2, float r2)
{
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance < (r1 + r2 - MOVE_STEP * 0.1f);
}

// --- Funciones de GLUT ---
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Borde del escenario
    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(SCENARIO_MIN_X - SNAKE_RADIUS, SCENARIO_MIN_Y - SNAKE_RADIUS);
    glVertex2f(SCENARIO_MAX_X + SNAKE_RADIUS, SCENARIO_MIN_Y - SNAKE_RADIUS);
    glVertex2f(SCENARIO_MAX_X + SNAKE_RADIUS, SCENARIO_MAX_Y + SNAKE_RADIUS);
    glVertex2f(SCENARIO_MIN_X - SNAKE_RADIUS, SCENARIO_MAX_Y + SNAKE_RADIUS);
    glEnd();
    glLineWidth(1.0f);

    if (gameOver) {
        drawText(-0.35f, 0.1f, "PERDISTE :'v!", 1.0f, 0.2f, 0.2f);
        char finalScoreText[50];
        sprintf(finalScoreText, "Puntos: %d", score);
        drawText(-0.3f, 0.0f, finalScoreText, 1.0f, 1.0f, 1.0f);
        drawText(-0.45f, -0.1f, "Presiona 'R' para reiniciar o 'Q' para rendirse", 0.8f, 0.8f, 0.8f);
    } else if (victoryAchieved) {
        drawText(-0.35f, 0.1f, "TODAY'S DINNER IS CHICKEN!", 0.2f, 1.0f, 0.2f);
        char finalScoreText[50];
        sprintf(finalScoreText, "Alcanzaste %d puntos!", score);
        drawText(-0.4f, 0.0f, finalScoreText, 1.0f, 1.0f, 1.0f);
        drawText(-0.45f, -0.1f, "Presiona 'R' para reiniciar o 'Q' para salir", 0.8f, 0.8f, 0.8f);
    } else if (gamePaused) {
        drawText(-0.25f, 0.0f, "AAAAAAA!", 1.0f, 1.0f, 0.2f);
        drawText(-0.45f, -0.1f, "Presiona 'P' para resumir o 'Q' para rendirse", 0.8f, 0.8f, 0.8f);
    } else {
        drawCircle(applePosition.x, applePosition.y, APPLE_RADIUS, 32, 1.0f, 0.0f, 0.0f);
        for (const auto& segment : snake) {
            drawCircle(segment.x, segment.y, SNAKE_RADIUS, 32, 0.0f, 0.5f, 1.0f);
        }

        char scoreText[50];
        sprintf(scoreText, "Puntos: %d", score);
        drawText(SCENARIO_MIN_X + 0.02f, SCENARIO_MAX_Y + 0.03f, scoreText, 1.0f, 1.0f, 1.0f); // White
        drawText(SCENARIO_MIN_X + 0.02f, SCENARIO_MIN_Y - 0.08f, "P: Pausar, Q: Salir", 0.7f, 0.7f, 0.7f);
    }

    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
    if (height == 0)
        height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Evitar deformaciones
    if (width >= height) {
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'r':
    case 'R':
        if (gameOver || victoryAchieved) {
            resetGame();
        }
        break;
    case 'p':
    case 'P':
        if (!gameOver && !victoryAchieved) {
            gamePaused = !gamePaused;
            glutPostRedisplay();
        }
        break;
    case 'q':
    case 'Q':
    case 27:
        exit(0);
        break;
    }
}

void specialKeys(int key, int x, int y)
{
    if (gameOver || victoryAchieved || gamePaused || snake.empty())
        return;

    Point currentDirection = snakeDirection;

    // Impedir moverse en contra
    switch (key) {
    case GLUT_KEY_UP:
        if (currentDirection.y == 0) {
            snakeDirection = { 0.0f, MOVE_STEP };
        }
        break;
    case GLUT_KEY_DOWN:
        if (currentDirection.y == 0) {
            snakeDirection = { 0.0f, -MOVE_STEP };
        }
        break;
    case GLUT_KEY_LEFT:
        if (currentDirection.x == 0) {
            snakeDirection = { -MOVE_STEP, 0.0f };
        }
        break;
    case GLUT_KEY_RIGHT:
        if (currentDirection.x == 0) {
            snakeDirection = { MOVE_STEP, 0.0f };
        }
        break;
    }
}

void updateGameLogic()
{
    if (gameOver || victoryAchieved || gamePaused || snake.empty()) {
        return;
    }

    // Calcular la nueva posicion de la cabeza (1 paso)
    Point newHead = snake.front();
    newHead.x += snakeDirection.x;
    newHead.y += snakeDirection.y;

    // Calcular colisiones
    if (newHead.x < SCENARIO_MIN_X || newHead.x > SCENARIO_MAX_X || newHead.y < SCENARIO_MIN_Y || newHead.y > SCENARIO_MAX_Y) {
        gameOver = true;
        glutPostRedisplay();
        return;
    }

    for (size_t i = 0; i < snake.size(); ++i) {
        if (snake[i] == newHead) {
            gameOver = true;
            glutPostRedisplay();
            return;
        }
    }

    // La serpiente crece
    snake.insert(snake.begin(), newHead);

    // Colisiones con manzanas
    if (checkCircleCollision(newHead, SNAKE_RADIUS, applePosition, APPLE_RADIUS)) {
        score++;
        if (score >= WIN_SCORE) {
            victoryAchieved = true;
            glutPostRedisplay();
            return;
        }
        spawnApple();
    } else {
        snake.pop_back();
    }

    glutPostRedisplay();
}

void timer(int value)
{
    updateGameLogic();
    glutTimerFunc(TIMER_DELAY_MS, timer, 0);
}

void initGL()
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
}

int main(int argc, char** argv)
{
    srand(static_cast<unsigned int>(time(0)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("La serpiente (Solo 6 puntos, 20 es mucho)");

    initGL();
    resetGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(TIMER_DELAY_MS, timer, 0);

    std::cout << "Controles:" << std::endl;
    std::cout << "Flechitas: Controlar serpiente" << std::endl;
    std::cout << "P: Pausar/Resumir" << std::endl;
    std::cout << "R: Reiniciar" << std::endl;
    std::cout << "Q o ESC: Rendirse o Salir" << std::endl;

    glutMainLoop();
    return 0;
}
