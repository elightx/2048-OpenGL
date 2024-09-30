#include<vector>
#include<algorithm>
#include<cstdlib>
#pragma once

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

private:
    
    const int GRID_SIZE;
    std::vector<std::vector<int>> GRID;
    const int TARGET;
    int SCORE = 0;
    int MAX_SCORE = 0;
    Status STATUS = Status::CONTINUE;

    std::vector<int> mergeSingle(std::vector<int> &row) {
        std::vector<int> res(1, row.front());
        bool added = 0;
        for(int j = 1; j < row.size(); j++) {
            if(res.back() == row[j] && !added) {
                res.back()++;
                SCORE += 1<<res.back();
                added = 1;
            } 
            else {
                res.push_back(row[j]);
                added = 0;
            }
        }
        while(res.size() < GRID_SIZE) {
            res.push_back(0);
        }
        return res;
    }

    int setSingle(std::vector<int> &res, bool is_row, int i) {
        int moved = 0;
        for(int j = 0; j < GRID_SIZE; j++) {
            int &x = is_row ? GRID[i][j] : GRID[j][i];
            if(x != res[j]) {
                moved = 1;
            }
            x = res[j];
        }
        return moved;
    }

    bool moveLeft() {
        bool moved = 0;
        for(int i = 0; i < GRID_SIZE; i++) {
            std::vector<int> row;
            for(int j = 0; j < GRID_SIZE; j++) {
                if(GRID[i][j] != 0) {
                    row.push_back(GRID[i][j]);
                }
            }
            if(row.empty())
                continue;
            
            std::vector<int> res = mergeSingle(row);
            
            moved+=setSingle(res, 1, i);
        }
        return moved;
    }

    bool moveRight() {
        bool moved = 0;
        for(int i = 0; i < GRID_SIZE; i++) {
            std::vector<int> row;
            for(int j = GRID_SIZE - 1; j >= 0; j--) {
                if(GRID[i][j] != 0) {
                    row.push_back(GRID[i][j]);
                }
            }
            if(row.empty())
                continue;

            std::vector<int> res = mergeSingle(row);

            reverse(res.begin(), res.end());

            moved+=setSingle(res, 1, i);
            
        }
        return moved;
    }
    bool moveUp() {
        bool moved = 0;
        for(int i = 0; i < GRID_SIZE; i++) {
            std::vector<int> col;
            for(int j = 0; j < GRID_SIZE; j++) {
                if(GRID[j][i] != 0) {
                    col.push_back(GRID[j][i]);
                }
            }
            if(col.empty())
                continue;
            
            std::vector<int> res = mergeSingle(col);

            moved+=setSingle(res, 0, i);
        }
        return moved;
    }
    bool moveDown() {
        bool moved = 0;
        for(int i = 0; i < GRID_SIZE; i++) {
            std::vector<int> col;
            for(int j = GRID_SIZE - 1; j >= 0; j--) {
                if(GRID[j][i] != 0) {
                    col.push_back(GRID[j][i]);
                }
            }
            if(col.empty())
                continue;
            
            std::vector<int> res = mergeSingle(col);
            reverse(res.begin(), res.end());
            
            moved+=setSingle(res, 0, i);
        }
        return moved;
    }

    bool move(Direction d){
        switch (d) {
            case Direction::UP:
                return moveUp();
            case Direction::DOWN:
                return moveDown();
            case Direction::LEFT:
                return moveLeft();
            case Direction::RIGHT:
                return moveRight();
            default:
                return 0;
        }
    }


    void genRandom() {
        MAX_SCORE = std::max(MAX_SCORE, SCORE);
        int emptyCells = 0;
        for(int i = 0; i < GRID_SIZE; i++) {
            for(int j = 0; j < GRID_SIZE; j++) {
                if(GRID[i][j] == 0) {
                    emptyCells++;
                } else if(GRID[i][j] == TARGET && STATUS == Status::CONTINUE) {
                    STATUS = Status::WIN;
                }
            }
        }
        if(emptyCells == 0) {
            return;
        }
        int randomCell = rand() % emptyCells;
        int value = (rand() % 10 == 9) + 1;

        for(int i = 0; i < GRID_SIZE; i++) {
            for(int j = 0; j < GRID_SIZE; j++) {
                if(GRID[i][j] == 0 && randomCell-- == 0) {
                    GRID[i][j] = value;
                    return;
                }
            }
        }
    }

public:

    Game(int grid_size, int target) : GRID_SIZE(grid_size), GRID(grid_size, std::vector<int>(grid_size, 0)), TARGET(target) {
        genRandom();
    }    

    std::vector<std::vector<int>>  GetGrid() {
        return GRID;
    }

    int GetScore() {
        return SCORE;
    }

    int GetMaxScore() {
        return MAX_SCORE;
    }

    int GetCell(int x, int y) {
        return GRID[y][x];
    }

    Status GetStatus() {
        return STATUS;
    }

    void Continue() {
        if(STATUS == Status::WIN) {
            STATUS = Status::WIN_CONTINUE;
        } 
    }

    void Restart() {
        GRID = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
        SCORE = 0;
        STATUS = Status::CONTINUE;
        genRandom();
    }

    void CheckLose() {
        for(int i = 0; i < GRID_SIZE; i++) {
            for(int j = 0; j < GRID_SIZE; j++) {
                if(GRID[i][j] == 0) {
                    return;
                }
                if(i < GRID_SIZE - 1 && GRID[i][j] == GRID[i + 1][j]) {
                    return;
                }
                if(j < GRID_SIZE - 1 && GRID[i][j] == GRID[i][j + 1]) {
                    return;
                }
            }
        }
        STATUS = Status::LOSE;
    }

    void Move(Direction d) {
        if(STATUS != Status::CONTINUE && STATUS != Status::WIN_CONTINUE) {
            return;
        }
        if(move(d)) {
            genRandom();
        }
        CheckLose();
    }

};
