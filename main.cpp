#include "include/platform.hpp"
#include "include/game.hpp"
#include <cstdlib>
#include <ctime>

int main() {
    platform::init();
    srand(static_cast<unsigned>(time(nullptr)));

    Game game;
    game.run();

    platform::restore();
    return 0;
}
