#pragma once

#include "board.hpp"
#include "tetromino.hpp"
#include "highscore.hpp"
#include "modes.hpp"
#include <memory>

class Game {
public:
    Game();
    void run();

    void setMode(std::shared_ptr<IMode> m) { mode = m; }
    HighscoreManager &getHighscoreManager() { return highscoreManager; }

private:
    Board board;
    Tetromino current;
    Tetromino next;
    bool gameOver;
    int tick;

    int score;
    int level;
    int totalLinesCleared; // cumulative lines cleared
    int ticksPerDrop; // controls how often pieces auto-drop (in ticks)

    static constexpr int baseTicksPerDrop = 10;
    static constexpr int linesPerLevel = 10;

    HighscoreManager highscoreManager;
    std::shared_ptr<IMode> mode;

    void drawNextPiece() const;
    void hardDrop();
    void onLinesCleared(int cleared);
};
