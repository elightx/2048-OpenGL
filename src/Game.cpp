//
// Created by 2163a on 04-02-2025.
//

#include "Game.h"


Game::Game(int grid_size, int target) : GRID_SIZE(grid_size),
        GRID(grid_size, std::vector<int>(grid_size, 0)),
        CHANGE(grid_size, std::vector<int>(grid_size, -1)),
        PAST(grid_size, std::vector<int>(grid_size, -1)),
        TARGET(target)
{
    srand(time(0));
    genRandom();
    genRandom();
}

Game::History::History(std::vector<std::vector<int>> past, std::vector<std::vector<int>> after_past, std::vector<std::vector<int>> change, Direction direction)
:
            PAST(past),
            AFTER_PAST(after_past),
            CHANGE(change),
            DIRECTION(direction), TIME(std::chrono::system_clock::now().time_since_epoch().count())
{}
std::vector<std::vector<int>>  Game::GetGrid()
{
    return GRID;
}
int Game::GetScore()
{
    return SCORE;
}
int Game::GetMaxScore()
{
    return MAX_SCORE;
}
int Game::GetCell(int x, int y)
{
    return GRID[y][x];
}
Game::Status Game::GetStatus()
{
    return STATUS;
}
void Game::Continue()
{
    if (STATUS == Status::WIN) {
        STATUS = Status::WIN_CONTINUE;
    }
}
void Game::Restart()
{
    GRID = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    SCORE = 0;
    STATUS = Status::CONTINUE;
    genRandom();
    genRandom();
}
void Game::CheckLose()
{
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (GRID[i][j] == 0) {
                return;
            }
            if (i < GRID_SIZE - 1 && GRID[i][j] == GRID[i + 1][j]) {
                return;
            }
            if (j < GRID_SIZE - 1 && GRID[i][j] == GRID[i][j + 1]) {
                return;
            }
        }
    }
    STATUS = Status::LOSE;
}
bool Game::Changed()
{
    return PAST != GRID;
}
void Game::popHistory()
{
    if (!HISTORY.empty()) HISTORY.pop();
}
Game::History Game::GetPAST()
{
    return HISTORY.front();
}
bool Game::hasPAST()
{
    return !HISTORY.empty();
}
size_t Game::pastCOUNT()
{
    return HISTORY.size();
}
long long Game::getLastTime()
{
    return LAST_MOVE_TIME;
}
void Game::Move(Direction d)
{
    if (STATUS != Status::CONTINUE && STATUS != Status::WIN_CONTINUE) {
        return;
    }
    PAST = GRID;
    move(d);
    if (Changed()) {
        genRandom();
        HISTORY.emplace(PAST, GRID, CHANGE, d);
        LAST_MOVE_TIME = std::chrono::system_clock::now().time_since_epoch().count();
    }
    CheckLose();
}
void Game::mergeSingle(std::vector<int>& row, std::vector<int>& prev)
{
    std::vector<int> res(1, row.front()), next(GRID_SIZE, -1);
    bool added = false;
    next[prev.front()] = 0;
    int i = 0;
    for (int j = 1; j < row.size(); j++) {
        if (res.back() == row[j] && !added) {
            res.back()++;
            SCORE += 1 << res.back();
            added = true;
            next[prev[j]] = i;
        }
        else {
            next[prev[j]] = ++i;
            res.push_back(row[j]);
            added = false;
        }
    }
    while (res.size() < GRID_SIZE) {
        res.push_back(0);
    }
    while (next.size() < GRID_SIZE) {
        next.push_back(-1);
    }
    prev = next;
    row = res;
}
void Game::setSingle(std::vector<int>& res, std::vector<int>& next, bool is_row, int i)
{
    for (int j = 0; j < GRID_SIZE; j++) {
        int& x = is_row ? GRID[i][j] : GRID[j][i];
        int& c = is_row ? CHANGE[i][j] : CHANGE[j][i];
        c = next[j];
        x = res[j];
    }
}
void Game::moveLeft()
{
    for (int i = 0; i < GRID_SIZE; i++) {
        std::vector<int> row, prev;
        for (int j = 0; j < GRID_SIZE; j++) {
            if (GRID[i][j] != 0) {
                row.push_back(GRID[i][j]);
                prev.push_back(j);
            }
        }
        if (row.empty())
            continue;

        mergeSingle(row, prev);

        setSingle(row, prev, 1, i);
    }
}
void Game::moveRight()
{
    for (int i = 0; i < GRID_SIZE; i++) {
        std::vector<int> row, prev;
        for (int j = GRID_SIZE - 1; j >= 0; j--) {
            if (GRID[i][j] != 0) {
                row.push_back(GRID[i][j]);
                prev.push_back(j);
            }
        }
        if (row.empty())
            continue;

        mergeSingle(row, prev);

        std::reverse(row.begin(), row.end());
        for (auto& p : prev) if(p!=-1) p = GRID_SIZE - 1 - p;

        setSingle(row, prev, 1, i);

    }
}
void Game::moveUp()
{
    for (int i = 0; i < GRID_SIZE; i++) {
        std::vector<int> col, prev;
        for (int j = 0; j < GRID_SIZE; j++) {
            if (GRID[j][i] != 0) {
                col.push_back(GRID[j][i]);
                prev.push_back(j);
            }
        }
        if (col.empty())
            continue;

        mergeSingle(col, prev);
		setSingle(col, prev, 0, i);
    }
}
void Game::moveDown()
{
    for (int i = 0; i < GRID_SIZE; i++) {
        std::vector<int> col, prev;
        for (int j = GRID_SIZE - 1; j >= 0; j--) {
            if (GRID[j][i] != 0) {
                col.push_back(GRID[j][i]);
                prev.push_back(j);
            }
        }
        if (col.empty())
            continue;

        mergeSingle(col, prev);
        std::reverse(col.begin(), col.end());
        for (auto& p : prev) if (p != -1) p = GRID_SIZE - 1 - p;


        setSingle(col, prev, 0, i);
    }
}
void Game::move(Direction d)
{
    switch (d) {
    case Direction::UP:
        moveUp();
        break;
    case Direction::DOWN:
        moveDown();
        break;
    case Direction::LEFT:
        moveLeft();
        break;
    case Direction::RIGHT:
        moveRight();
        break;
    default:
        break;
    }
}
void Game::genRandom()
{
    MAX_SCORE = std::max(MAX_SCORE, SCORE);
    int emptyCells = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (GRID[i][j] == 0) {
                emptyCells++;
            }
            else if (GRID[i][j] == TARGET && STATUS == Status::CONTINUE) {
                STATUS = Status::WIN;
            }
        }
    }
    if (emptyCells == 0) {
        return;
    }
    int randomCell = rand() % emptyCells;
    int value = (rand() % 10 == 9) + 1;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (GRID[i][j] == 0 && randomCell-- == 0) {
                GRID[i][j] = value;
                return;
            }
        }
    }
}