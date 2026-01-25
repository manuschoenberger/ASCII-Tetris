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

    void setMode(std::shared_ptr<IMode> m) { mode = std::move(m); }
    HighscoreManager &getHighscoreManager() { return highscoreManager; }

    void scheduleNextSpeedMultiplier(int m) { nextSpeedMultiplier = m; speedNotePending = (m > 1); }

    int getScore() const { return score; }

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

    int nextSpeedMultiplier = 1; // multiplier to apply to next piece (default 1)
    int activeSpeedMultiplier = 1; // multiplier currently in effect for the active piece

    // note flags: pending means scheduled for next piece; active means the current piece is affected
    bool speedNotePending = false;
    bool speedNoteActive = false;

    void drawNextPiece() const;
    void hardDrop();
    void onLinesCleared(int cleared);
};
