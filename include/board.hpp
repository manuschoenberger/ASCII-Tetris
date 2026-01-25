#pragma once

#include "tetromino.hpp"
#include <string>

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;

class Board {
public:
    int grid[BOARD_HEIGHT][BOARD_WIDTH];

    Board();

    // optional right-side note (e.g. warnings) will be printed to the right of the board header
    void draw(int score, int level, int highscore, const std::string &note = "") const;

    void drawPiece(const Tetromino &t);
    void clearPiece();

    bool collides(const Tetromino &t) const;
    void lockPiece(const Tetromino &t);
    int clearLines();
};
