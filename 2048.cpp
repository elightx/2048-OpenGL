#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <bits/stdc++.h>
#include <sys/time.h>

using namespace std;
const int GRID_SIZE = 4;

float scale = 1;

float bg_colour[] = {250, 248, 239};
float border = GRID_SIZE * GRID_SIZE * scale;
float border_colour[] = {187, 173, 160};

float window_w = 600, window_h = 800;
float game_w = 512 * scale, game_h = 512 * scale;

float box_size = 108 * scale;

int score = 0, maxscore = 0;

float boxcolor[13][3] = {{204, 192, 179}, {238, 228, 218}, {237, 224, 200},
                         {242, 177, 121}, {245, 149, 99},  {246, 124, 95},
                         {246, 94, 59},   {237, 207, 114}, {237, 204, 97},
                         {237, 200, 80},  {237, 197, 63},  {237, 194, 46},
                         {0, 0, 0}};

bool lose = false, win = false, started = false, continue_game = false;

const int TARGET = 11;
// const int TARGET = 4;
vector<vector<int>> grid(GRID_SIZE, vector<int>(GRID_SIZE, 0));

enum Direction { UP, LEFT, DOWN, RIGHT, NONE };

vector<int> add(vector<int> v) {
    vector<int> res = {0};
    bool k = 1;
    for (int p : v) {
        if (!res.empty() and res.back() == 0) {
            k = 1;
            res.back() += p;
        } else if (!res.empty() and p == res.back() and k) {
            k = 0;
            res.back()++;
            res.push_back(0);
        } else {
            k = 1;
            res.push_back(p);
        }
    }
    while (res.size() < GRID_SIZE) res.push_back(0);
    return res;
}

vector<vector<int>> action(Direction d) {
    vector<vector<int>> temp = grid;
    if (d == UP) {
        // move up
        for (int j = 0; j < GRID_SIZE; j++) {
            vector<int> v;
            for (int i = 0; i < GRID_SIZE; i++) {
                if (grid[i][j] != 0) {
                    v.push_back(grid[i][j]);
                }
            }
            vector<int> res = add(v);
            for (int i = 0; i < GRID_SIZE; i++) {
                temp[i][j] = res[i];
            }
        }

    } else if (d == LEFT) {
        // move left
        for (int i = 0; i < GRID_SIZE; i++) {
            vector<int> v;
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] != 0) {
                    v.push_back(grid[i][j]);
                }
            }
            vector<int> res = add(v);
            for (int j = 0; j < GRID_SIZE; j++) {
                temp[i][j] = res[j];
            }
        }
    } else if (d == DOWN) {
        // move down
        for (int j = 0; j < GRID_SIZE; j++) {
            vector<int> v;
            for (int i = GRID_SIZE - 1; i >= 0; i--) {
                if (grid[i][j] != 0) {
                    v.push_back(grid[i][j]);
                }
            }
            vector<int> res = add(v);
            for (int i = GRID_SIZE - 1; i >= 0; i--) {
                temp[i][j] = res[GRID_SIZE - 1 - i];
            }
        }
    } else if (d == RIGHT) {
        // move right
        for (int i = 0; i < GRID_SIZE; i++) {
            vector<int> v;
            for (int j = GRID_SIZE - 1; j >= 0; j--) {
                if (grid[i][j] != 0) {
                    v.push_back(grid[i][j]);
                }
            }
            vector<int> res = add(v);
            for (int j = GRID_SIZE - 1; j >= 0; j--) {
                temp[i][j] = res[GRID_SIZE - 1 - j];
            }
        }
    }
    return temp;
}

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
    maxscore = max(score, maxscore);
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(-230, 275, 0);
    float f = 0.8;
    glScalef(f, f, f);
    glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char *)"2048");
    glPopMatrix();

    string x = to_string(score);
    if (x.empty()) x = "0";
    string ss = "Score : " + x;
    x = to_string(maxscore);
    if (x.empty()) x = "0";
    string ms = "Max Score : " + x;
    glRasterPos2i(100, 300);
    for (auto p : ss) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, p);
    glRasterPos2i(100, 270);
    for (auto p : ms) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, p);
    // glFlush();
}

void notification(string s, vector<float> col) {
    glColor4f(col[0], col[1], col[2], 0.5);
    // glRectf(-game_w/2, -game_h/2, game_w/2, game_h/2);
    glRectf(-game_w, -game_h / 10, game_w, game_h / 10);
    // drawRectWithRoundedCorners(-game_w/2,-game_h/2,game_w/2,game_h/2,20);
    glColor3f(1.0, 1.0, 1.0);
    int n = s.size();
    int pos = -35 * n / 9 * scale;
    // glTranslatef(-100,0,0);
    // glScalef(0.15,0.15,0.15);

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

void genRandom(Direction d, vector<vector<int>> temp = grid) {
    if (grid == temp and
        grid != vector<vector<int>>(GRID_SIZE, vector<int>(GRID_SIZE, 0)))
        return;
    grid = temp;
    int tempscore = 0;
    for (auto i : grid)
        for (auto j : i) tempscore += j < 2 ? 0 : (1 << j);
    score = tempscore;
    // generate a new number
    srand(time(NULL));
    int num = (rand() % 10 >= 9) + 1;
    int empty = 0;
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++) empty += grid[i][j] == 0;

    if (!empty) return;

    int pos = rand() % empty;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                if (pos == 0) {
                    grid[i][j] = num;
                }
                pos--;
            } else if (grid[i][j] == TARGET) {
                win = true;
            }
        }
    }
    if (action(UP) == grid and action(DOWN) == grid and action(LEFT) == grid and
        action(RIGHT) == grid)
        lose = true;

    glutPostRedisplay();
}

void myInit(void) {
    score = 0;
    lose = false;
    win = false;
    started = false;
    continue_game = false;
    grid = vector<vector<int>>(GRID_SIZE, vector<int>(GRID_SIZE, 0));
    glClearColor(1.0, 1.0, 1.0, 0.25);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 0.0, 0.0);
    glPointSize(1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-window_w * scale / 2, window_w * scale / 2,
               -window_h * scale / 2, window_h * scale / 2);
    genRandom(NONE);
}

void display() {
    glClearColor(bg_colour[0], bg_colour[1], bg_colour[2], 1);
    glClear(GL_COLOR_BUFFER_BIT);
    // draw background
    glColor3fv(border_colour);
    // glRecti(-game_w/2,-game_h/2,game_w/2,game_h/2);
    drawRectWithRoundedCorners(-game_w / 2, -game_h / 2, game_w / 2, game_h / 2,
                               20);

    // draw the numbers
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        int x = i % GRID_SIZE;
        int y = i / GRID_SIZE;
        glColor3fv(boxcolor[grid[y][x]]);
        drawBox(x, y);
        if (grid[y][x] == 0) continue;
        if (grid[y][x] < 3)
            glColor3f(119.0 / 255, 110.0 / 255, 101.0 / 255);
        else
            glColor3f(1.0, 1.0, 1.0);
        glRasterPos2i(
            -game_w / 2 + border * (x + 1) + box_size / 2 + box_size * x,
            game_h / 2 - border * (y + 1) - box_size / 2 - box_size * y);
        string s = to_string(1 << grid[y][x]);
        for (int i = 0; i < s.size(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
        }
    }
    ShowScore();
    if (started) {
        glRasterPos2i(-100, -300);
        glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,
                         (unsigned char *)"Press R for New Game");
    }
    if (win and !continue_game) {
        notification("You Win! Press C to continue", {0.0, 1.0, 0.0});
    }
    if (lose) {
        notification("GAME OVER", {1.0, 0.0, 0.0});
    }
    if (!started) {
        started = true;
        // notification("Press Space to start",{0.0, 0.0, 1.0});
    }
    glFlush();
}

void keyboardHandler(unsigned char key, int x, int y) {
    vector<vector<int>> temp = grid;

    if (key >= 'A' and key <= 'Z') key = key - 'A' + 'a';

    Direction d = NONE;
    if (key == 'w') d = UP;
    if (key == 'a') d = LEFT;
    if (key == 's') d = DOWN;
    if (key == 'd') d = RIGHT;

    if (key == 'q') {
        exit(1);
    } else if (key == 'r') {
        myInit();
        glutPostRedisplay();
    } else if (d != NONE) {
        genRandom(d, action(d));
    } else if (key == 'c' and win) {
        continue_game = true;
        glutPostRedisplay();
    }
}

void specialKeyboardHandler(int key, int x, int y) {
    Direction d = NONE;
    if (key == GLUT_KEY_UP) d = UP;
    if (key == GLUT_KEY_LEFT) d = LEFT;
    if (key == GLUT_KEY_DOWN) d = DOWN;
    if (key == GLUT_KEY_RIGHT) d = RIGHT;

    if (d != NONE) genRandom(d, action(d));
}

int lastX = 0, lastY = 0;

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        int dx = x - lastX;
        int dy = y - lastY;
        float theta = atan2(dy, dx);

        if (-M_PI / 4 <= theta and theta <= M_PI / 4)
            genRandom(RIGHT, action(RIGHT));
        else if (M_PI / 4 <= theta and theta <= 3 * M_PI / 4)
            genRandom(DOWN, action(DOWN));
        else if (-M_PI / 4 >= theta and theta >= -3 * M_PI / 4)
            genRandom(UP, action(UP));
        else
            genRandom(LEFT, action(LEFT));
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastX = x;
        lastY = y;
    }
}

// void mouseDrag(int x, int y) {
//     if (dragging) return;
// }

int main(int argc, char **argv) {
    for (int i = 0; i < 3; i++) bg_colour[i] /= 255;
    for (int i = 0; i < 3; i++) border_colour[i] /= 255;
    for (int i = 0; i < 12; i++)
        for (int j = 0; j < 3; j++) boxcolor[i][j] /= 255;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    // glEnable(GL_BLEND);

    // glBlendFunc(GL_SRC_ALPHA,1);
    // glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
    glutInitWindowSize(window_w, window_h);
    glutInitWindowPosition(900, 100);

    glutCreateWindow("2048");
    myInit();
    glutSpecialFunc(specialKeyboardHandler);
    glutKeyboardFunc(keyboardHandler);
    glutMouseFunc(mouseClick);
    // glutMotionFunc(mouseDrag);
    glutDisplayFunc(display);
    glutMainLoop();
}
