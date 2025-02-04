//
// Created by 2163a on 04-02-2025.
//

#ifndef GAME_H
#define GAME_H

#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <queue>

class Game {
public:
    enum Status {
        WIN_CONTINUE,
        CONTINUE,
        WIN,
        LOSE
    };
    enum Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        NONE
    };
    struct History
    {
        std::vector<std::vector<int>> PAST;
        std::vector<std::vector<int>> AFTER_PAST;
        std::vector<std::vector<int>> CHANGE;
        Direction DIRECTION;
        long long TIME;
        History(std::vector<std::vector<int>> past, std::vector<std::vector<int>> after_past, std::vector<std::vector<int>> change, Direction direction);
    };
    Game();
    Game(int grid_size, int target);
    std::vector<std::vector<int>>  GetGrid() ;
    int GetScore();
    int GetMaxScore();
    int GetCell(int x, int y);
    Status GetStatus();
    void Continue();
    void Restart();
    void CheckLose();
    bool Changed();
    void popHistory();
    History GetPAST();
    bool hasPAST();
    size_t pastCOUNT();
    long long getLastTime();
    void Move(Direction d) ;
private:
    const int GRID_SIZE;
    std::vector<std::vector<int>> GRID;
    std::vector<std::vector<int>> CHANGE;
    std::vector<std::vector<int>> PAST;
    const int TARGET;
    int SCORE = 0;
    int MAX_SCORE = 0;
    Status STATUS = Status::CONTINUE;
    std::queue<History> HISTORY;
    long long LAST_MOVE_TIME;
    void mergeSingle(std::vector<int>& row, std::vector<int>& prev);
    void setSingle(std::vector<int>& res, std::vector<int>& next, bool is_row, int i);
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void move(Direction d);
    void genRandom();
};



#endif //GAME_H
