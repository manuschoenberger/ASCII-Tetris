#pragma once

#include "board.hpp"
#include "tetromino.hpp"

class Game {
public:
    Game();
    void run();

private:
    Board board;
    Tetromino current;
    Tetromino next;
    bool gameOver;
    int tick;

    void drawNextPiece() const;
    void hardDrop();
};

