#pragma once

#include "highscore.hpp"

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

    Selection run();

private:
    HighscoreManager &highscoreManager;

    void render(int highlight) const;
};

