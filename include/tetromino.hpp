#pragma once

struct Tetromino {
    int shape[4][4];
    int x;
    int y;
};

Tetromino createRandomPiece();

void rotateClockwise(Tetromino &t);
