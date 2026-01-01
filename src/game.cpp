#include "../include/game.hpp"
#include "../include/platform.hpp"
#include "../include/tetromino.hpp"
#include <iostream>
#include <thread>
#include <chrono>

Game::Game(): current(), next(), gameOver(false), tick(0) {
    current = createRandomPiece();
    current.x = BOARD_WIDTH / 2 - 2; // center the piece
    current.y = 0;

    next = createRandomPiece(); // next piece
}

void Game::drawNextPiece() const {
    std::cout << "\n Next:\n";

    for (int y = 0; y < 4; ++y) {
        std::cout << "  ";

        for (int x = 0; x < 4; ++x) {
            if (next.shape[y][x] == 1)
                std::cout << " #";
            else
                std::cout << " .";
        }

        std::cout << "\n";
    }
}

void Game::hardDrop() {
    Tetromino temp = current;

    while (true) {
        Tetromino nextpos = temp;
        nextpos.y++;
        if (board.collides(nextpos)) break;
        temp = nextpos;
    }

    current = temp;
    board.lockPiece(current);
    board.clearLines();
}

void Game::run() {
    std::cout << "\033[?25l"; // hide cursor
    std::cout << "\033[2J"; // clear screen once

    while (!gameOver) {
        board.clearPiece();

        if (platform::kbhit()) {
            int c = platform::getch();
            Tetromino temp = current;

            if (c == 'a') {
                temp.x--;
                if (!board.collides(temp)) current = temp;
            } else if (c == 'd') {
                temp.x++;
                if (!board.collides(temp)) current = temp;
            } else if (c == 's') {
                temp.y++;
                if (!board.collides(temp)) current = temp;
            } else if (c == 'w') {
                Tetromino rot = current;
                rotateClockwise(rot);

                if (!board.collides(rot)) current = rot;
                else {
                    static const int kicks[] = { -1, 1, -2, 2, -3, 3 };

                    for (int kick : kicks) {
                        rot.x += kick;

                        if (!board.collides(rot)) {
                            current = rot; break;
                        }

                        rot.x -= kick;
                    }
                }
            } else if (c == ' ') {
                hardDrop();
                current = next;
                current.x = BOARD_WIDTH / 2 - 2;
                current.y = 0;
                next = createRandomPiece();
                if (board.collides(current)) gameOver = true;
            }
        }

        if (tick % 10 == 0) {
            Tetromino temp = current;
            temp.y++;

            if (!board.collides(temp)) current = temp;
            else {
                board.lockPiece(current);
                board.clearLines();
                current = next;
                current.x = BOARD_WIDTH / 2 - 2;
                current.y = 0;
                next = createRandomPiece();
                if (board.collides(current)) gameOver = true;
            }
        }

        board.drawPiece(current);
        board.draw();
        drawNextPiece();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        ++tick;
    }

    std::cout << "\nGAME OVER!\n";
    std::cout << "\033[?25h"; // show cursor
}

