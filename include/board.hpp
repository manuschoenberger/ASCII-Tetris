#pragma once

#include "tetromino.hpp"

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;

class Board {
public:
    int grid[BOARD_HEIGHT][BOARD_WIDTH];

    Board();

    void draw() const; // draw to console

    void drawPiece(const Tetromino &t);
    void clearPiece();

    bool collides(const Tetromino &t) const;
    void lockPiece(const Tetromino &t);
    int clearLines();
};

