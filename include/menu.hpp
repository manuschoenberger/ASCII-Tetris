#pragma once

#include "highscore.hpp"
#include <memory>

class Menu {
public:
    enum class Selection {
        Normal,
        Fun,
        Hard,
        Mixed,
        Quit
    };

    explicit Menu(HighscoreManager &hs);

    Selection run(); // Runs the menu loop and returns the user's selection.

private:
    HighscoreManager &highscoreManager;

    void render(int highlight) const;
};

