#include <iostream>
#include <thread>
#include <chrono>
#include <conio.h>

// Board dimensions
static const int WIDTH = 10;
static const int HEIGHT = 20;

class Board {
public:
    int grid[HEIGHT][WIDTH] = {0};

    void draw() {
        std::cout << "\033[2J\033[H"; // clear screen on Windows

        std::cout << "---------- ASCII TETRIS ----------\n";
        for (int y = 0; y < HEIGHT; y++) {
            std::cout << "|";
            for (int x = 0; x < WIDTH; x++) {
                if (grid[y][x] == 0) std::cout << " .";
                else std::cout << " #";
            }
            std::cout << " |\n";
        }
        std::cout << "----------------------------------\n";
    }
};

// 4×4 Tetromino matrix
struct Tetromino {
    int shape[4][4];
    int x;
    int y;
};

Tetromino createIPiece() {
    Tetromino t{
        {
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        3, 0 // starting position
    };
    return t;
}

// Check collisions
bool collides(Board &board, Tetromino &t) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (t.shape[i][j] == 1) {
                int bx = t.x + j;
                int by = t.y + i;

                // out of bounds
                if (bx < 0 || bx >= WIDTH) return true;
                if (by < 0 || by >= HEIGHT) return true;

                // collision with existing blocks
                if (board.grid[by][bx] != 0) return true;
            }
        }
    }
    return false;
}

// Place Tetromino onto board (when locked)
void lockPiece(Board &board, Tetromino &t) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (t.shape[i][j] == 1) {
                board.grid[t.y + i][t.x + j] = 1;
            }
        }
    }
}

// Draw tetromino temporarily without locking
void drawPiece(Board &board, Tetromino &t) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (t.shape[i][j] == 1) {
                int bx = t.x + j;
                int by = t.y + i;
                if (by >= 0 && by < HEIGHT && bx >= 0 && bx < WIDTH) {
                    // Temporarily draw the piece
                    board.grid[by][bx] = 2;
                }
            }
        }
    }
}

void clearPiece(Board &board) {
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            if (board.grid[y][x] == 2)
                board.grid[y][x] = 0;
}

int main() {
    Board board;
    Tetromino piece = createIPiece();

    bool gameOver = false;
    int tick = 0;

    while (!gameOver) {
        clearPiece(board);

        // --- INPUT ---
        if (_kbhit()) {
            char c = _getch();

            Tetromino temp = piece;

            if (c == 'a') {      // left
                temp.x--;
                if (!collides(board, temp)) piece = temp;
            }
            else if (c == 'd') { // right
                temp.x++;
                if (!collides(board, temp)) piece = temp;
            }
            else if (c == 's') { // down
                temp.y++;
                if (!collides(board, temp)) piece = temp;
            }
        }

        // --- GRAVITY ---
        if (tick % 10 == 0) {
            Tetromino temp = piece;
            temp.y++;

            if (!collides(board, temp)) {
                piece = temp;
            } else {
                // lock the piece
                lockPiece(board, piece);

                // spawn new piece
                piece = createIPiece();
                if (collides(board, piece)) {
                    gameOver = true;
                }
            }
        }

        // Draw the falling piece
        drawPiece(board, piece);

        // Render
        board.draw();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        tick++;
    }

    std::cout << "\nGAME OVER!\n";
    return 0;
}
