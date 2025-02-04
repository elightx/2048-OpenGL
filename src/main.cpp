#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>

#include "Game.h"
#include "FontManager.h"

#define CLEAR_SANS "ClearSans.ttf"
#define CLEAR_SANS_BOLD "ClearSansBold.ttf"

FontManager* font_manager = new FontManager();

constexpr int GRID_SIZE = M_GRID_SIZE;
constexpr int TARGET = M_TARGET;

const long long TILE_MOVE_DURATION = M_TILE_MOVE_DURATION;
const long long TILE_RECOIL_DURATION = M_TILE_RECOIL_DURATION;

auto game = new Game(GRID_SIZE, TARGET);

GLfloat scale = 1;
GLfloat bg_colour[] = { 250.0f/255, 248.0f/255, 239.0f/255, 1.0f };
GLfloat dark_colour[] = { 119.0f / 255, 110.0f / 255, 101.0f / 255, 1.0f };
GLfloat white[] = { 1.0f,1.0f,1.0f, 1.0f };
GLfloat border_colour[] = { 187.0f/255, 173.0f/255, 160.0f/255, 1.0f };
GLfloat window_w = 800, window_h = 800;
GLfloat game_w = 512 * scale, game_h = 512 * scale;
GLfloat border = game_w / (GRID_SIZE * GRID_SIZE * 2) * scale;
GLfloat box_size = (game_w- border*(GRID_SIZE+1))/GRID_SIZE * scale;
GLfloat box_color[13][3] = {{204, 192, 179}, {238, 228, 218}, {237, 224, 200},
                            {242, 177, 121}, {245, 149, 99}, {246, 124, 95},
                            {246, 94,  59}, {237, 207, 114}, {237, 204, 97},
                            {237, 200, 80}, {237, 197, 63}, {237, 194, 46},
                            {0,   0,   0} };


void drawRectWithRoundedCorners(GLfloat* color , float cx, float cy, float dx, float dy, float r) {
	glColor3fv(color);
	r *= scale;
	float x0, y0, x, y, a = 0.0;
	const int n = 9;
	const float da = 1.5707963267948966192313216916398 / float(n);
	dx -= r + r;
	dy -= r + r;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);
	x0 = cx + (0.5 * dx);
	y0 = cy + (0.5 * dy);
	for (int i = 0; i < n; i++, a += da) {
		x = x0 + (r * cos(a));
		y = y0 + (r * sin(a));
		glVertex2f(x, y);
	}
	x0 -= dx;
	for (int i = 0; i < n; i++, a += da) {
		x = x0 + (r * cos(a));
		y = y0 + (r * sin(a));
		glVertex2f(x, y);
	}
	y0 -= dy;
	for (int i = 0; i < n; i++, a += da) {
		x = x0 + (r * cos(a));
		y = y0 + (r * sin(a));
		glVertex2f(x, y);
	}
	x0 += dx;
	for (int i = 0; i < n; i++, a += da) {
		x = x0 + (r * cos(a));
		y = y0 + (r * sin(a));
		glVertex2f(x, y);
	}
	glVertex2f(x, cy + (0.5 * dy));
	glEnd();
}

void drawTileNumber(double x, double y, int number)
{
	if (number == 1) return;
	std::string s = std::to_string(number);
	GLfloat centerX;
	GLfloat centerY;

	if (GRID_SIZE % 2 == 0)
	{
		centerX = window_w / 2 + (x - (GRID_SIZE / 2 - 1)) * (box_size + border) - (box_size + border) / 2;
		centerY = window_h / 2 - (y - (GRID_SIZE / 2 - 1)) * (box_size + border) + (box_size + border) / 2;
	}
	else
	{
		centerX = window_w / 2 + (x - GRID_SIZE / 2) * (box_size + border);
		centerY = window_h / 2 - (y - GRID_SIZE / 2) * (box_size + border);
	}

	std::vector<int> fonts_sizes = {64, 56, 48, 40, 32, 24, 16, 8};
	int font_index = 0;
	while (font_index<fonts_sizes.size() && font_manager->GetStringWidth(s, CLEAR_SANS_BOLD, fonts_sizes[font_index]) * 1.1>box_size) font_index++;
	const int font_size = fonts_sizes[font_index];
	GLfloat* num_color = (number < 8) ? dark_colour : white;
	GLfloat font_height = font_manager->GetLineHeight(CLEAR_SANS_BOLD, font_size);
	GLfloat font_width = font_manager->GetStringWidth(s, CLEAR_SANS_BOLD, font_size);
	font_manager->draw_string(s, CLEAR_SANS_BOLD, font_size, centerX - font_width / 2, centerY - font_height/2, num_color);
}


void drawBox(GLfloat* color, double x, double y, int number, int Box_Size = box_size) {
	drawRectWithRoundedCorners(
		color,
		-game_w / 2 + border + box_size / 2 + (border + box_size) * x,
		game_h / 2 - border - box_size / 2 - (border + box_size) * y,
		Box_Size,
		Box_Size,
		5);
	drawTileNumber(x, y, number);
}

void ShowScore() {
	constexpr int font_size = 32;
	constexpr GLfloat width_padding = 15;
	constexpr GLfloat height_padding = 10;
	constexpr GLfloat window_padding = 5;
	const GLfloat score_box_width = window_w / 2 - window_padding * 2;
	const GLfloat score_font_height = font_manager->GetLineHeight(CLEAR_SANS, font_size);
	const GLfloat score_box_height = 2 * height_padding + score_font_height;
	const GLfloat score_bottom_pos_y = window_h / 2 - 125;
	drawRectWithRoundedCorners(white, -window_w/4, score_bottom_pos_y + score_font_height/2 + 10, score_box_width, score_box_height, 20);
	drawRectWithRoundedCorners(white, window_w / 4, score_bottom_pos_y + score_font_height/2 + 10, score_box_width, score_box_height, 20);
	const std::string score_text = std::to_string(game->GetScore());
	const std::string max_score_text = std::to_string(game->GetMaxScore());
	const GLfloat score_x = window_w/2 - window_padding - width_padding - font_manager->GetStringWidth(score_text, CLEAR_SANS, font_size);
	const GLfloat score_y = score_bottom_pos_y + window_h / 2 + height_padding;
	const GLfloat max_score_x = window_w - window_padding - width_padding - font_manager->GetStringWidth(max_score_text, CLEAR_SANS, font_size);
	const GLfloat max_score_y = score_y;
	font_manager->draw_string("SCORE", CLEAR_SANS, font_size, window_padding + width_padding, score_y, dark_colour);
	font_manager->draw_string(score_text, CLEAR_SANS, font_size, score_x, score_y, dark_colour);
	font_manager->draw_string("BEST", CLEAR_SANS, font_size, window_w / 2 + window_padding + width_padding, max_score_y, dark_colour);
	font_manager->draw_string(max_score_text, CLEAR_SANS, font_size, max_score_x, max_score_y, dark_colour);
}

void ShowLogo()
{
	constexpr int font_size = 64;
	const auto text = "2048";
	const GLfloat text_x = window_w / 2 - font_manager->GetStringWidth(text, CLEAR_SANS, font_size) / 2;
	const GLfloat text_y = window_h - font_manager->GetLineHeight(CLEAR_SANS, font_size);
	font_manager->draw_string(text, CLEAR_SANS, font_size, text_x, text_y, dark_colour);
}



void ShowRestartOption()
{
	const auto text = "Restart (R)";
	constexpr int font_size = 32;
	constexpr GLfloat height_padding = 10;
	constexpr GLfloat width_padding = 20;
	const GLfloat text_x = window_w - width_padding - font_manager->GetStringWidth(text,CLEAR_SANS, font_size);
	const GLfloat text_y = window_h - height_padding - font_manager->GetLineHeight(CLEAR_SANS, font_size);
	font_manager->draw_string(text, CLEAR_SANS, font_size, text_x, text_y, dark_colour);
}

void ShowExitOption()
{
	const auto text = "Exit (Q)";
	constexpr int font_size = 32;

	constexpr GLfloat height_padding = 10;
	constexpr GLfloat width_padding = 20;
	constexpr GLfloat text_x = width_padding;
	const GLfloat text_y = window_h - height_padding - font_manager->GetLineHeight(CLEAR_SANS, font_size);
	font_manager->draw_string(text, CLEAR_SANS, font_size, text_x, text_y, dark_colour);
}

void ShowContinueOption()
{
	const char* text= "Continue (C)";
	constexpr int font_size = 32;

	const GLfloat text_x = window_w/2 - font_manager->GetStringWidth(text, CLEAR_SANS, font_size)/2;
	constexpr GLfloat text_y = 100;
	font_manager->draw_string(text, CLEAR_SANS, font_size, text_x, text_y, dark_colour);

}

void ShowNotification(const char * s, const std::vector<float>& col) {
	constexpr int font_size = 64;
	GLfloat text_y = window_h / 2 - font_manager->GetLineHeight(CLEAR_SANS, font_size)/2;
	GLfloat text_x = window_w / 2 - font_manager->GetStringWidth(s, CLEAR_SANS, font_size)/2;
	glColor4f(col[0], col[1], col[2], 0.5);
	glRectf(-game_w, -game_h / 13, game_w, game_h / 13);
	font_manager->draw_string(s, CLEAR_SANS, font_size, text_x, text_y, white);
}

void myInit()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-window_w * scale / 2, window_w * scale / 2, -window_h * scale / 2, window_h * scale / 2);
}

void Display(GLFWwindow* window)
{
	font_manager->clear();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(bg_colour[0], bg_colour[1], bg_colour[2], 1.0);
	ShowLogo();
	ShowScore();
	ShowExitOption();
	ShowRestartOption();
	drawRectWithRoundedCorners(border_colour , 0, 0, game_w, game_h,20*4/GRID_SIZE);
	const long long current_time = std::chrono::system_clock::now().time_since_epoch().count();
	while (game->hasPAST() && (game->getLastTime() + TILE_MOVE_DURATION - game->GetPAST().TIME) / game->pastCOUNT() < current_time - game->GetPAST().TIME)
	{
		game->popHistory();
	}
	if (game->hasPAST())
	{
		Game::History past = game->GetPAST();
		long long allotted_time = (game->getLastTime() + TILE_MOVE_DURATION - past.TIME) / game->pastCOUNT();
		long long done_time = current_time - past.TIME;
		double tile_recoil_duration = TILE_RECOIL_DURATION / game->pastCOUNT();
		double shrink_progress = 0;
		double progress = (double)done_time / (allotted_time - tile_recoil_duration);
		if (progress > 1)
		{
			progress = 1 + abs(tile_recoil_duration / 2 - (done_time - (allotted_time - tile_recoil_duration))) / tile_recoil_duration / 19;
			shrink_progress = abs(tile_recoil_duration / 2 - (done_time - (allotted_time - tile_recoil_duration))) / tile_recoil_duration * 50 * 4 / GRID_SIZE;
		}

		for (int j = 0; j < GRID_SIZE; j++)
		{
			for (int i = 0; i < GRID_SIZE; i++)
			{
				drawBox(box_color[0], i, j, 1);

			}
		}
		for (int j = 0; j < GRID_SIZE; j++)
		{
			for (int i = 0; i < GRID_SIZE; i++)
			{
				if (past.PAST[j][i] == 0) continue;
				int x = i, y = j;
				if (past.DIRECTION == Game::Direction::DOWN || past.DIRECTION == Game::Direction::UP) y = past.CHANGE[j][i];
				else x = past.CHANGE[j][i];
				if (past.PAST[j][i] == past.AFTER_PAST[y][x] || progress < 1)
				{
					drawBox(box_color[past.PAST[j][i]], i + (x - i) * progress, j + (y - j) * progress, 1 << past.PAST[j][i]);
				}
				else
				{
					drawBox(box_color[past.AFTER_PAST[y][x]], x, y, 1 << past.AFTER_PAST[y][x], box_size + shrink_progress);
				}

			}
		}
	}
	else
	{
		// draw the numbers
		for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
			int x = i % GRID_SIZE;
			int y = i / GRID_SIZE;
			drawBox(box_color[game->GetCell(x, y)], x, y, 1 << game->GetCell(x, y));
		}
		if (game->GetStatus() == Game::Status::WIN) {
			ShowNotification("You Win!", { 0.0, 1.0, 0.0 });
			ShowContinueOption();
		}
		if (game->GetStatus() == Game::Status::LOSE) {
			ShowNotification("Game Over", { 1.0, 0.0, 0.0 });
		}
	}
	font_manager->use_shader();
	font_manager->reshape(window_w, window_h);
	font_manager->render();
	glUseProgram(0);
	glfwSwapBuffers(window);
}


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_W:
		case GLFW_KEY_UP:
			game->Move(Game::Direction::UP);
			break;
		case GLFW_KEY_S:
		case GLFW_KEY_DOWN:
			game->Move(Game::Direction::DOWN);
			break;
		case GLFW_KEY_A:
		case GLFW_KEY_LEFT:
			game->Move(Game::Direction::LEFT);
			break;
		case GLFW_KEY_D:
		case GLFW_KEY_RIGHT:
			game->Move(Game::Direction::RIGHT);
			break;
		case GLFW_KEY_R:
			game->Restart();
			break;
		case GLFW_KEY_C:
			game->Continue();
			break;
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		default: ;
		}
	}
}



int dragging = 0;
GLfloat lastX = 0, lastY = 0;

void MouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	GLdouble x, y;
	glfwGetCursorPos(window, &x, &y);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		const GLfloat dx = x - lastX;
		const GLfloat dy = y - lastY;
		if (dx * dx + dy * dy < 100) return;
		auto theta = static_cast<GLfloat>(atan2(dy, dx));
		if (-M_PI / 4 <= theta && theta <= M_PI / 4)
			game->Move(Game::Direction::RIGHT);
		else if (M_PI / 4 <= theta && theta <= 3 * M_PI / 4)
			game->Move(Game::Direction::DOWN);
		else if (-M_PI / 4 >= theta && theta >= -3 * M_PI / 4)
			game->Move(Game::Direction::UP);
		else
			game->Move(Game::Direction::LEFT);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		lastX = x;
		lastY = y;
	}
}

void MouseMotionCallback(GLFWwindow* window, double x, double y)
{
	if (dragging) return;
}
void Resize(GLFWwindow* window, int width, int height)
{
	window_w = width;
	window_h = height;
	glViewport(0, 0, width, height);
	font_manager->reshape(window_w, window_h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-window_w * scale / 2, window_w * scale / 2,
		-window_h * scale / 2, window_h * scale / 2);
}

int main(int argc, char** argv)
{
	for (int i = 0; i < 12; i++)
		for (int j = 0; j < 3; j++) box_color[i][j] /= 255;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(window_w, window_h, "2048", nullptr, nullptr);

	if (!window)
	{
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(window, Resize);
	glfwSetWindowRefreshCallback(window, Display);
	glfwSetWindowSizeCallback(window, Resize);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseClickCallback);
	glfwSetCursorPosCallback(window, MouseMotionCallback);
	glewInit();
	myInit();
	font_manager->init();

	while (!glfwWindowShouldClose(window))
	{
		Display(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	delete game;
	delete font_manager;
	glfwTerminate();
	return 0;
}
