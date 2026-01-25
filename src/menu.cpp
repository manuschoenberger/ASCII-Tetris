#include "../include/menu.hpp"
#include "../include/platform.hpp"
#include <iostream>
#include <vector>
#include <string>

Menu::Menu(HighscoreManager &hs): highscoreManager(hs) {}

void Menu::render(int highlight) const {
    std::cout << "\033[2J"; // clear screen
    std::cout << "\033[H"; // move cursor home

    std::cout << "===== ASCII TETRIS - MAIN MENU =====\n\n";
    std::cout << "Highscore: " << highscoreManager.getHighscore() << "\n\n";

    std::vector<std::string> options = { "Normal Mode", "Fun Mode", "Hard Mode", "Mixed Mode (NOT AVAILABLE)", "Quit" };

    for (size_t i = 0; i < options.size(); ++i) {
        if (static_cast<int>(i) == highlight) std::cout << "> "; else std::cout << "  ";
        std::cout << (i + 1) << ". " << options[i] << "\n";
    }

    std::cout << "\nUse number keys or arrow keys then Enter to select.\n";
}

Menu::Selection Menu::run() {
    int highlight = 0;

    render(highlight);

    while (true) {
        if (platform::kbhit()) {
            int c = platform::getch();

            if (c == '1') return Selection::Normal;
            if (c == '2') return Selection::Fun;
            if (c == '3') return Selection::Hard;
            if (c == '4') return Selection::Mixed;
            if (c == '5' || c == 'q' || c == 'Q') return Selection::Quit;

            // handle arrow keys
            if (c == 0 || c == 224) {
                int c2 = platform::getch();
                if (c2 == 72) { // up
                    highlight = std::max(0, highlight - 1);
                    render(highlight);
                } else if (c2 == 80) { // down
                    highlight = std::min(4, highlight + 1);
                    render(highlight);
                }
            } else if (c == '\r' || c == '\n') {
                // Enter: choose current highlight
                switch (highlight) {
                    case 0: return Selection::Normal;
                    case 1: return Selection::Fun;
                    case 2: return Selection::Hard;
                    case 3: return Selection::Mixed;
                    default: return Selection::Quit;
                }
            }
        }
    }
}

