#include <iostream>
#include <thread>
#include <chrono>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>

// Board dimensions
static const int WIDTH = 10;
static const int HEIGHT = 20;

class Board {
public:
    int grid[HEIGHT][WIDTH] = {0};

    void draw() {
        std::cout << "\033[H"; // clear screen on Windows

        std::cout << "┌---- ASCII TETRIS ---┐\n";
        for (int y = 0; y < HEIGHT; y++) {
            std::cout << "|";
            for (int x = 0; x < WIDTH; x++) {
                if (grid[y][x] == 0) std::cout << " .";
                else std::cout << " #";
            }
            std::cout << " |\n";
        }
        std::cout << "└---------------------┘\n";
        std::cout.flush();
    }
};

// 4×4 Tetromino matrix
struct Tetromino {
    int shape[4][4];
    int x;
    int y;
};

enum class TetrominoType {
    I, O, T, S, Z, J, L
};

const int TETROMINO_SHAPES[7][4][4] = {
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

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            t.shape[i][j] = TETROMINO_SHAPES[type][i][j];
        }
    }

    t.x = WIDTH / 2 - 2;
    t.y = 0;

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

void rotateClockwise(Tetromino& t) {
    int temp[4][4] = {0};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[j][3 - i] = t.shape[i][j];

    // never rotate blindly, instead copy back only if valid
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            t.shape[i][j] = temp[i][j];
}

void tryRotate(Board& board, Tetromino& piece) {
    Tetromino temp = piece;
    rotateClockwise(temp);

    if (!collides(board, temp)) {
        piece = temp;
        return;
    }

    static const int kicks[] = { -1, 1, -2, 2, -3, 3 }; // wall kick offsets

    for (int kick : kicks) {
        temp.x += kick;

        if (!collides(board, temp)) {
            piece = temp;
            return;
        }

        temp.x -= kick; // revert
    }
}

void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void enableUTF8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

void clearLines(Board& board) {
    // Check each line from bottom to top
    for (int y = 0; y < HEIGHT; y++) {
        bool fullLine = true;
        for (int x = 0; x < WIDTH; x++) {
            if (board.grid[y][x] == 0) {
                fullLine = false;
                break;
            }
        }
        if (fullLine) {
            // Clear the line and move everything above down
            for (int ty = y; ty > 0; ty--) {
                for (int tx = 0; tx < WIDTH; tx++) {
                    board.grid[ty][tx] = board.grid[ty - 1][tx];
                }
            }
            // Clear the top line
            for (int tx = 0; tx < WIDTH; tx++) {
                board.grid[0][tx] = 0;
            }
        }
    }
}

void hardDrop(Board& board, Tetromino& piece) {
    Tetromino temp = piece;

    while (true) {
        Tetromino next = temp;
        next.y++;

        if (collides(board, next)) {
            break;
        }

        temp = next;
    }

    // Lock at final position
    piece = temp;
    lockPiece(board, piece);
    clearLines(board);
}

void drawNextPiece(const Tetromino& t) {
    std::cout << "\n Next:\n";

    for (int y = 0; y < 4; y++) {
        std::cout << "  ";
        for (int x = 0; x < 4; x++) {
            if (t.shape[y][x] == 1)
                std::cout << " #";
            else
                std::cout << " .";
        }
        std::cout << "\n";
    }
}

int main() {
    enableUTF8(); // enable UTF-8 for box drawing characters
    srand(static_cast<unsigned>(time(nullptr)));

    Board board;
    Tetromino currentPiece = createRandomPiece();
    Tetromino nextPiece = createRandomPiece();

    bool gameOver = false;
    int tick = 0;

    std::cout << "\033[?25l"; // hide cursor
    std::cout << "\033[2J"; // clear screen once

    while (!gameOver) {
        clearPiece(board);

        // --- INPUT ---
        if (_kbhit()) {
            char c = _getch();

            Tetromino temp = currentPiece;

            if (c == 'a') {
                temp.x--;
                if (!collides(board, temp)) currentPiece = temp;
            } else if (c == 'd') {
                temp.x++;
                if (!collides(board, temp)) currentPiece = temp;
            } else if (c == 's') {
                temp.y++;
                if (!collides(board, temp)) currentPiece = temp;
            } else if (c == 'w') {
                tryRotate(board, currentPiece);
            } else if (c == ' ') {
                hardDrop(board, currentPiece);

                currentPiece = nextPiece;
                currentPiece.x = WIDTH / 2 - 2;
                currentPiece.y = 0;

                nextPiece = createRandomPiece();

                if (collides(board, currentPiece)) {
                    gameOver = true;
                }
            }
        }

        // --- GRAVITY ---
        if (tick % 10 == 0) {
            Tetromino temp = currentPiece;
            temp.y++;

            if (!collides(board, temp)) {
                currentPiece = temp;
            } else {
                // lock the piece
                lockPiece(board, currentPiece);
                clearLines(board);

                // spawn new piece
                currentPiece = nextPiece;
                currentPiece.x = WIDTH / 2 - 2;
                currentPiece.y = 0;

                nextPiece = createRandomPiece();

                if (collides(board, currentPiece)) {
                    gameOver = true;
                }
            }
        }

        // Draw the falling piece
        drawPiece(board, currentPiece);
        board.draw();
        drawNextPiece(nextPiece);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        tick++;
    }

    std::cout << "\nGAME OVER!\n";
    std::cout << "\033[?25h"; // show cursor again
    return 0;
}
