#include "../include/board.hpp"
#include <iostream>

Board::Board() {
    for (int y = 0; y < BOARD_HEIGHT; ++y)
        for (int x = 0; x < BOARD_WIDTH; ++x)
            grid[y][x] = 0;
}

void Board::draw() const {
    std::cout << "\033[H"; // move cursor to home
    std::cout << "┌---- ASCII TETRIS ---┐\n";

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        std::cout << "|";

        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (grid[y][x] == 0) std::cout << " ."; // empty
            else if (grid[y][x] == 1) std::cout << " #"; // locked piece
            else std::cout << " @"; // current piece
        }

        std::cout << " |\n";
    }

    std::cout << "└---------------------┘\n";
    std::cout.flush();
}

void Board::drawPiece(const Tetromino &t) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (t.shape[i][j] == 1) {
                int bx = t.x + j;
                int by = t.y + i;

                if (by >= 0 && by < BOARD_HEIGHT && bx >= 0 && bx < BOARD_WIDTH)
                    grid[by][bx] = 2;
            }
        }
    }
}

void Board::clearPiece() {
    for (int y = 0; y < BOARD_HEIGHT; ++y)
        for (int x = 0; x < BOARD_WIDTH; ++x)
            if (grid[y][x] == 2)
                grid[y][x] = 0;
}

bool Board::collides(const Tetromino &t) const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (t.shape[i][j] == 1) {
                int bx = t.x + j;
                int by = t.y + i;

                // check bounds
                if (bx < 0 || bx >= BOARD_WIDTH) return true;
                if (by < 0 || by >= BOARD_HEIGHT) return true;

                if (grid[by][bx] != 0) return true; // collision with locked piece
            }
        }
    }

    return false;
}

void Board::lockPiece(const Tetromino &t) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (t.shape[i][j] == 1) {
                int bx = t.x + j;
                int by = t.y + i;

                if (by >= 0 && by < BOARD_HEIGHT && bx >= 0 && bx < BOARD_WIDTH)
                    grid[by][bx] = 1;
            }
}

int Board::clearLines() {
    int cleared = 0;

    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        bool full = true;

        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (grid[y][x] == 0) {
                full = false;
                break;
            }
        }

        if (full) {
            ++cleared;

            for (int ty = y; ty > 0; --ty)
                for (int tx = 0; tx < BOARD_WIDTH; ++tx)
                    grid[ty][tx] = grid[ty - 1][tx];

            for (int tx = 0; tx < BOARD_WIDTH; ++tx)
                grid[0][tx] = 0;
        }
    }

    return cleared;
}

