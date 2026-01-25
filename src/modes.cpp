#include "../include/modes.hpp"
#include "../include/game.hpp"
#include <memory>
#include <cstdlib>

namespace {
    struct BaseMode : IMode {
        std::string name() const override { return "Base"; }
    };

    struct NormalMode : BaseMode {
        std::string name() const override { return "Normal"; }
    };

    struct FunMode : BaseMode {
        std::string name() const override { return "Fun"; }
    };

    struct HardMode : BaseMode {
        std::string name() const override { return "Hard"; }

        void onLock(Game &game) override {
            if (game.getScore() < 500) return; // Negative power-ups start after 500 points

            // 10% chance to trigger a negative
            int r = std::rand() % 100;
            if (r >= 10) return;

            game.scheduleNextSpeedMultiplier(3);
        }
    };

    struct MixedMode : BaseMode {
        std::string name() const override { return "Mixed"; }
    };
}

std::shared_ptr<IMode> createNormalMode() { return std::make_shared<NormalMode>(); }
std::shared_ptr<IMode> createFunMode() { return std::make_shared<FunMode>(); }
std::shared_ptr<IMode> createHardMode() { return std::make_shared<HardMode>(); }
std::shared_ptr<IMode> createMixedMode() { return std::make_shared<MixedMode>(); }
