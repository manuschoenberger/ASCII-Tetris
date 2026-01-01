#include "../include/tetromino.hpp"

#include <cstdlib>

static const int TETROMINO_SHAPES[7][4][4] = {
    // I
    {
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    // O
    {
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // T
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // S
    {
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Z
    {
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // J
    {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // L
    {
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }
};

Tetromino createRandomPiece() {
    Tetromino t{};
    int type = rand() % 7;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            t.shape[i][j] = TETROMINO_SHAPES[type][i][j];

    t.x = 0;
    t.y = 0;

    return t;
}

// Rotation works by transposing the matrix and then reversing each row.
void rotateClockwise(Tetromino &t) {
    int temp[4][4] = {0};

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            temp[j][3 - i] = t.shape[i][j];

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            t.shape[i][j] = temp[i][j];
}

