#include "../include/game.hpp"
#include "../include/platform.hpp"
#include "../include/tetromino.hpp"
#include <iostream>
#include <thread>
#include <chrono>

Game::Game(): current(), next(), gameOver(false), tick(0), score(0), level(0), totalLinesCleared(0), ticksPerDrop(baseTicksPerDrop), highscoreManager("highscore.txt"), nextSpeedMultiplier(1), activeSpeedMultiplier(1), speedNotePending(false), speedNoteActive(false), slowPiecesRemaining(0), slowActiveForCurrent(false), slowFactorActive(1) {
    current = createRandomPiece();
    current.x = BOARD_WIDTH / 2 - 2; // center the piece
    current.y = 0;

    next = createRandomPiece(); // next piece
}

void Game::fillBottomHole() {
    for (int y = BOARD_HEIGHT - 1; y >= 0; --y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (board.grid[y][x] == 0) {
                board.grid[y][x] = 1;
                return;
            }
        }
    }
}

void Game::activateSlowForSpawnedPiece() {
    if (slowPiecesRemaining > 0) {
        slowActiveForCurrent = true;
        slowPiecesRemaining -= 1;
    } else {
        slowActiveForCurrent = false;
    }
}

void Game::skipCurrentPiece() {
    current = next;
    current.x = BOARD_WIDTH / 2 - 2;
    current.y = 0;
    next = createRandomPiece();

    activateSlowForSpawnedPiece(); // when skipping, the new current is considered a newly spawned piece -> activate slow for it if available
}

void Game::applySlowToActivePiece(int factor) {
    if (factor <= 1) return;
    slowPiecesRemaining += 3;
    slowFactorActive = factor;

    // immediately apply to the currently active piece as well
    if (!slowActiveForCurrent && slowPiecesRemaining > 0) {
        slowActiveForCurrent = true;
        slowPiecesRemaining -= 1;
    }
}

void Game::deleteTopRows(int n) {
    if (n <= 0) return;
    int removed = 0;
    for (int y = 0; y < BOARD_HEIGHT && removed < n; ++y) {
        bool hasLocked = false;
        for (int x = 0; x < BOARD_WIDTH; ++x) if (board.grid[y][x] == 1) { hasLocked = true; break; }
        if (hasLocked) {
            for (int ty = y; ty > 0; --ty)
                for (int tx = 0; tx < BOARD_WIDTH; ++tx)
                    board.grid[ty][tx] = board.grid[ty - 1][tx];
            for (int tx = 0; tx < BOARD_WIDTH; ++tx) board.grid[0][tx] = 0;
            ++removed;
            --y; // re-check
        }
    }
}

void Game::drawNextPiece() const {
    std::cout << "\n Next:\n";

    for (int y = 0; y < 4; ++y) {
        std::cout << "  ";

        for (int x = 0; x < 4; ++x) {
            if (next.shape[y][x] == 1) std::cout << " #";
            else std::cout << " .";
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

    // if the piece that just locked had the 3x-speed effect active, consume it and clear the note
    if (speedNoteActive) {
        speedNoteActive = false;
        activeSpeedMultiplier = 1;
    }
}

void Game::run() {
    std::cout << "\033[?25l"; // hide cursor
    std::cout << "\033[2J"; // clear screen once

    // apply any scheduled speed effect before starting (unlikely at startup, but safe)
    if (nextSpeedMultiplier > 1) {
        activeSpeedMultiplier = nextSpeedMultiplier;
        nextSpeedMultiplier = 1;
        speedNoteActive = (activeSpeedMultiplier > 1);
        speedNotePending = false;
    }

    if (mode) mode->onStart(*this);

    activateSlowForSpawnedPiece(); // ensure the initial piece respects any already scheduled slow pieces

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

                if (mode) mode->onLock(*this); // before spawning next, allow mode to schedule effects for the upcoming piece

                // show indicator for the upcoming piece
                if (speedNotePending) {
                    board.draw(score, level, highscoreManager.getHighscore(), "3x speed for NEXT piece");
                    std::cout << std::flush;
                    std::this_thread::sleep_for(std::chrono::milliseconds(800));
                }

                activeSpeedMultiplier = nextSpeedMultiplier;
                nextSpeedMultiplier = 1;

                // pending note moves to active when the scheduled effect is applied
                speedNoteActive = activeSpeedMultiplier > 1;
                if (speedNoteActive) speedNotePending = false;

                current = next;
                current.x = BOARD_WIDTH / 2 - 2;
                current.y = 0;
                next = createRandomPiece();
                if (board.collides(current)) gameOver = true;

                activateSlowForSpawnedPiece(); // after spawn, activate slow if scheduled
            }

            if (mode) mode->onInput(*this, c);
        }

        // auto-drop logic
        int effectiveTicksPerDrop = ticksPerDrop;
        if (slowActiveForCurrent) effectiveTicksPerDrop = ticksPerDrop * slowFactorActive;

        if (tick % std::max(1, effectiveTicksPerDrop / activeSpeedMultiplier) == 0) {
            Tetromino temp = current;
            temp.y++;

            if (!board.collides(temp)) current = temp;
            else {
                board.lockPiece(current);
                int cleared = board.clearLines();
                if (cleared > 0) onLinesCleared(cleared);

                // if the piece that just locked had the 3x-speed effect active, consume it and clear the note
                if (speedNoteActive) {
                    speedNoteActive = false;
                    activeSpeedMultiplier = 1;
                }

                // if slow-note active for this piece, consume it now
                if (slowActiveForCurrent) {
                    slowActiveForCurrent = false;
                }

                if (mode) mode->onLock(*this); // allow mode to schedule an effect for the next piece

                // show indicator for the upcoming piece
                if (speedNotePending) {
                    board.draw(score, level, highscoreManager.getHighscore(), "3x speed for NEXT piece");
                    std::cout << std::flush;
                    std::this_thread::sleep_for(std::chrono::milliseconds(800));
                }

                activeSpeedMultiplier = nextSpeedMultiplier;
                nextSpeedMultiplier = 1;

                // pending note moves to active when the scheduled effect is applied
                speedNoteActive = activeSpeedMultiplier > 1;
                if (speedNoteActive) speedNotePending = false;

                current = next;
                current.x = BOARD_WIDTH / 2 - 2;
                current.y = 0;
                next = createRandomPiece();
                if (board.collides(current)) gameOver = true;

                activateSlowForSpawnedPiece(); // after spawn, activate slow if scheduled
            }
        }

        if (mode) mode->onTick(*this, tick);

        board.drawPiece(current);

        if (speedNoteActive) {
            board.draw(score, level, highscoreManager.getHighscore(), "3x speed ACTIVE");
        } else if (speedNotePending) {
            board.draw(score, level, highscoreManager.getHighscore(), "3x speed for NEXT piece");
        } else {
            std::string note = "";
            if (mode) note = mode->getSideNote(*this);
            board.draw(score, level, highscoreManager.getHighscore(), note);
        }

        drawNextPiece();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        ++tick;
    }

    std::cout << "\nGAME OVER! Game closes shortly.\n";

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
