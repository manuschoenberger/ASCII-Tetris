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

    // Fun-mode / mode effect helper APIs (minimal public surface)
    void fillBottomHole();
    void skipCurrentPiece();
    void applySlowToActivePiece(int factor); // now schedules the slow effect for the next N pieces (3 by default via FunMode)
    void deleteTopRows(int n);

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

    // slow effect state: how many upcoming pieces should be slowed, and whether current piece is slowed
    int slowPiecesRemaining = 0; // number of upcoming pieces that will be slowed
    bool slowActiveForCurrent = false; // whether the currently active piece is slowed
    int slowFactorActive = 1; // multiplier for slowing (e.g., 3)

    void drawNextPiece() const;
    void hardDrop();
    void onLinesCleared(int cleared);

    // helper to activate slow effect for newly spawned piece
    void activateSlowForSpawnedPiece();

    friend class IMode; // allow modes to call game.* helpers if needed
};
