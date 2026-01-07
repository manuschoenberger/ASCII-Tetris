#include "../include/game.hpp"
#include "../include/platform.hpp"
#include "../include/tetromino.hpp"
#include <iostream>
#include <thread>
#include <chrono>

Game::Game(): current(), next(), gameOver(false), tick(0), score(0), level(0), totalLinesCleared(0), ticksPerDrop(baseTicksPerDrop), highscoreManager("highscore.txt") {
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
    int cleared = board.clearLines();
    if (cleared > 0) onLinesCleared(cleared);
}

void Game::run() {
    std::cout << "\033[?25l"; // hide cursor
    std::cout << "\033[2J"; // clear screen once

    if (mode) mode->onStart(*this);

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

            if (mode) mode->onInput(*this, c);
        }

        if (tick % ticksPerDrop == 0) {
            Tetromino temp = current;
            temp.y++;

            if (!board.collides(temp)) current = temp;
            else {
                board.lockPiece(current);
                int cleared = board.clearLines();
                if (cleared > 0) onLinesCleared(cleared);
                current = next;
                current.x = BOARD_WIDTH / 2 - 2;
                current.y = 0;
                next = createRandomPiece();
                if (board.collides(current)) gameOver = true;
            }
        }

        if (mode) mode->onTick(*this, tick);

        board.drawPiece(current);
        board.draw(score, level, highscoreManager.getHighscore());
        drawNextPiece();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        ++tick;
    }

    std::cout << "\nGAME OVER!\n";

    if (highscoreManager.saveIfHigher(score)) {
        std::cout << "New highscore saved: " << score << "\n";
    } else {
        std::cout << "Highscore: " << highscoreManager.getHighscore() << "\n";
    }

    std::cout << std::flush;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "\033[?25h"; // show cursor
}

// Leveling and scoring rules
void Game::onLinesCleared(int cleared) {
    int points = 0;

    switch (cleared) {
        case 1: points = 40 * (level + 1); break;
        case 2: points = 100 * (level + 1); break;
        case 3: points = 300 * (level + 1); break;
        case 4: points = 1200 * (level + 1); break;
        default: points = 0; break;
    }

    score += points;
    totalLinesCleared += cleared;

    int newLevel = totalLinesCleared / linesPerLevel;

    if (newLevel > level) {
        level = newLevel;
        ticksPerDrop = std::max(1, baseTicksPerDrop - level);
    }
}
