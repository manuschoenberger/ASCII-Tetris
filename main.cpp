#include "include/platform.hpp"
#include "include/game.hpp"
#include "include/menu.hpp"
#include "include/modes.hpp"
#include <cstdlib>
#include <ctime>

int main() {
    platform::init();
    srand(static_cast<unsigned>(time(nullptr)));

    Game game;

    // show main menu
    Menu menu(game.getHighscoreManager());

    auto selection = menu.run();

    if (selection == Menu::Selection::Quit) {
        platform::restore();
        return 0;
    }

    std::shared_ptr<IMode> mode;
    switch (selection) {
        case Menu::Selection::Normal: mode = createNormalMode(); break;
        case Menu::Selection::Fun: mode = createFunMode(); break;
        case Menu::Selection::Hard: mode = createHardMode(); break;
        case Menu::Selection::Mixed: mode = createMixedMode(); break;
        default: mode = createNormalMode(); break;
    }

    game.setMode(mode);
    game.run();

    platform::restore();
    return 0;
}
