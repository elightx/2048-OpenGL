#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string>
#include <vector>
#include <cmath>
#include "game.cpp"

const int GRID_SIZE = 4;
const int TARGET = 11;

Game* game = new Game(GRID_SIZE, 11);

float scale = 1;
float bg_colour[] = {250, 248, 239};
float border = GRID_SIZE * GRID_SIZE * scale;
float border_colour[] = {187, 173, 160};

float window_w = 600, window_h = 800;
float game_w = 512 * scale, game_h = 512 * scale;

float box_size = 108 * scale;


float boxcolor[13][3] = {{204, 192, 179}, {238, 228, 218}, {237, 224, 200},
                         {242, 177, 121}, {245, 149, 99},  {246, 124, 95},
                         {246, 94, 59},   {237, 207, 114}, {237, 204, 97},
                         {237, 200, 80},  {237, 197, 63},  {237, 194, 46},
                         {0, 0, 0}};


void drawRectWithRoundedCorners(float xx, float xy, float wx, float wy,
                                float r) {
    int i;
    r *= scale;
    float cx = (xx + wx) / 2;
    float cy = (wy + xy) / 2;
    float dx = abs(wx - xx);
    float dy = abs(wy - xy);
    float x0, y0, x, y, a = 0.0;
    const int n = 9;
    const float da = 1.5707963267948966192313216916398 / float(n);
    dx -= r + r;
    dy -= r + r;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    x0 = cx + (0.5 * dx);
    y0 = cy + (0.5 * dy);
    for (i = 0; i < n; i++, a += da) {
        x = x0 + (r * cos(a));
        y = y0 + (r * sin(a));
        glVertex2f(x, y);
    }
    x0 -= dx;
    for (i = 0; i < n; i++, a += da) {
        x = x0 + (r * cos(a));
        y = y0 + (r * sin(a));
        glVertex2f(x, y);
    }
    y0 -= dy;
    for (i = 0; i < n; i++, a += da) {
        x = x0 + (r * cos(a));
        y = y0 + (r * sin(a));
        glVertex2f(x, y);
    }
    x0 += dx;
    for (i = 0; i < n; i++, a += da) {
        x = x0 + (r * cos(a));
        y = y0 + (r * sin(a));
        glVertex2f(x, y);
    }
    glVertex2f(x, cy + (0.5 * dy));
    glEnd();
}

void ShowScore() {
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(-230, 275, 0);
    float f = 0.8;
    glScalef(f, f, f);
    glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char *)"2048");
    glPopMatrix();

    const std::string score_text = "Score : " + std::to_string(game->GetScore());
    const std::string max_score_text = "Max Score : " + std::to_string(game->GetMaxScore());
    glRasterPos2i(100, 300);
    for (auto &p : score_text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, p);
    glRasterPos2i(100, 270);
    for (auto &p : max_score_text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, p);
    // glFlush();
}

void notification(std::string s, std::vector<float> col) {
    glColor4f(col[0], col[1], col[2], 0.5);
    glRectf(-game_w, -game_h / 10, game_w, game_h / 10);
    glColor3f(1.0, 1.0, 1.0);
    int n = s.size();
    int pos = -35 * n / 9 * scale;
    glRasterPos2i(pos, 0);
    for (int i = 0; i < s.size(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
    }
}

void drawBox(float x, float y) {
    drawRectWithRoundedCorners(-game_w / 2 + border + (border + box_size) * x,
                               game_w / 2 - border - (border + box_size) * y,
                               -game_h / 2 + (border + box_size) * (x + 1),
                               game_h / 2 - (border + box_size) * (y + 1), 5);
}

void myInit(void) {
    glClearColor(1.0, 1.0, 1.0, 0.25);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 0.0, 0.0);
    glPointSize(1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-window_w * scale / 2, window_w * scale / 2,
               -window_h * scale / 2, window_h * scale / 2);
}

void display() {
    glClearColor(bg_colour[0], bg_colour[1], bg_colour[2], 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3fv(border_colour);
    drawRectWithRoundedCorners(-game_w / 2, -game_h / 2, game_w / 2, game_h / 2,
                               20);

    // draw the numbers
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        int x = i % GRID_SIZE;
        int y = i / GRID_SIZE;
        glColor3fv(boxcolor[game->GetCell(x,y)]);
        drawBox(x, y);
        if (game->GetCell(x,y) == 0) continue;
        if (game->GetCell(x,y) < 3)
            glColor3f(119.0 / 255, 110.0 / 255, 101.0 / 255);
        else
            glColor3f(1.0, 1.0, 1.0);
        glRasterPos2i(
            -game_w / 2 + border * (x + 1) + box_size / 2 + box_size * x,
            game_h / 2 - border * (y + 1) - box_size / 2 - box_size * y);
        std::string s = std::to_string(1 << game->GetCell(x,y));
        for (int i = 0; i < s.size(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
        }
    }
    ShowScore();
    glRasterPos2i(-100, -300);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,
                        (unsigned char *)"Press R for New Game");
    if (game->GetStatus() == Game::Status::WIN) {
        notification("You Win! Press C to continue", {0.0, 1.0, 0.0});
    }
    if (game->GetStatus() == Game::Status::LOSE) {
        notification("GAME OVER", {1.0, 0.0, 0.0});
    }
    glFlush();
}

void keyboardHandler(unsigned char key, int x, int y) {
    if(!isalpha(key)) return;
    key = tolower(key);
    if (key == 'q') {
        exit(0);
    } else if (key == 'r') {
        game->Restart();
    } else if (key == 'c') {
        game->Continue();
    } 

    Game::Direction d = Game::Direction::NONE;
    if (key == 'w') d = Game::Direction::UP;
    if (key == 'a') d = Game::Direction::LEFT;
    if (key == 's') d = Game::Direction::DOWN;
    if (key == 'd') d = Game::Direction::RIGHT;
    game->Move(d);
    glutPostRedisplay();
}

void specialKeyboardHandler(int key, int x, int y) {
    Game::Direction d = Game::Direction::NONE;
    if (key == GLUT_KEY_UP) d = Game::Direction::UP;
    if (key == GLUT_KEY_LEFT) d = Game::Direction::LEFT;
    if (key == GLUT_KEY_DOWN) d = Game::Direction::DOWN;
    if (key == GLUT_KEY_RIGHT) d = Game::Direction::RIGHT;
    game->Move(d);
    glutPostRedisplay();
}

int dragging = 0;
int lastX = 0, lastY = 0;

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        int dx = x - lastX;
        int dy = y - lastY;
        float theta = atan2(dy, dx);

        if (-M_PI / 4 <= theta and theta <= M_PI / 4)
            game->Move(Game::Direction::RIGHT);
        else if (M_PI / 4 <= theta and theta <= 3 * M_PI / 4)
            game->Move(Game::Direction::DOWN);
        else if (-M_PI / 4 >= theta and theta >= -3 * M_PI / 4)
            game->Move(Game::Direction::UP);
        else
            game->Move(Game::Direction::LEFT);
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastX = x;
        lastY = y;
    }
    glutPostRedisplay();
}

void mouseDrag(int x, int y) {
    if (dragging) return;
}

int main(int argc, char **argv) {
    for (int i = 0; i < 3; i++) bg_colour[i] /= 255;
    for (int i = 0; i < 3; i++) border_colour[i] /= 255;
    for (int i = 0; i < 12; i++)
        for (int j = 0; j < 3; j++) boxcolor[i][j] /= 255;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(window_w, window_h);
    glutInitWindowPosition(900, 100);

    glutCreateWindow("2048");
    myInit();
    glutSpecialFunc(specialKeyboardHandler);
    glutKeyboardFunc(keyboardHandler);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseDrag);
    glutDisplayFunc(display);
    glutMainLoop();
}