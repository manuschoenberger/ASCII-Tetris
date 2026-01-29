#include "../include/modes.hpp"
#include "../include/game.hpp"
#include <memory>
#include <cstdlib>
#include <sstream>

namespace {
    struct BaseMode : IMode {
        std::string name() const override { return "Base"; }
    };

    struct NormalMode : BaseMode {
        std::string name() const override { return "Normal"; }
    };

    struct FunMode : BaseMode {
        struct Powerup {
            int pointsThreshold = 0;
            int cooldownNeeded = 0;
            int fixedSinceUse = -1; // -1 means not used / not currently cooling; >=0 counts fixed tetrominos since use
            bool ready = false;
        };

        FunMode() {
            p1.pointsThreshold = 1000; p1.cooldownNeeded = 15; p1.fixedSinceUse = -1; p1.ready = false;
            p2.pointsThreshold = 2500; p2.cooldownNeeded = 15; p2.fixedSinceUse = -1; p2.ready = false;
            p3.pointsThreshold = 5000; p3.cooldownNeeded = 15; p3.fixedSinceUse = -1; p3.ready = false;
            p4.pointsThreshold = 7500; p4.cooldownNeeded = 30; p4.fixedSinceUse = -1; p4.ready = false;
        }

        std::string name() const override { return "Fun"; }

        void onStart(Game &game) override {
            checkReadiness(game); // determine initial readiness based on score
        }

        void onTick(Game &game, int tick) override {
            (void)tick; (void)game;
        }

        void onInput(Game &game, int key) override {
            if (key >= '1' && key <= '4') {
                int idx = key - '1';
                attemptActivate(game, idx);
            }
        }

        void onLock(Game &game) override {
            auto inc = [&](Powerup &p) {
                if (p.fixedSinceUse >= 0) {
                    p.fixedSinceUse++;

                    // check if cooldown completed
                    if (p.fixedSinceUse >= p.cooldownNeeded) {
                        p.ready = true;
                        p.fixedSinceUse = -1;
                    }
                }
            };

            if (p1.fixedSinceUse >= 0) inc(p1);
            if (p2.fixedSinceUse >= 0) inc(p2);
            if (p3.fixedSinceUse >= 0) inc(p3);
            if (p4.fixedSinceUse >= 0) inc(p4);

            checkReadiness(game);
        }

        std::string getSideNote(const Game &game) const override {
            std::ostringstream ss;

            bool anyReady = p1.ready || p2.ready || p3.ready || p4.ready;
            if (!anyReady) return std::string();

            if (p1.ready) ss << "1) Fill bottom hole (press 1)" << "\n";
            if (p2.ready) ss << "2) Skip current piece (press 2)" << "\n";
            if (p3.ready) ss << "3) Slow current piece x3 (press 3)" << "\n";
            if (p4.ready) ss << "4) Remove top 3 rows (press 4)" << "\n";

            return ss.str();
        }

    private:
        Powerup p1, p2, p3, p4;

        void checkReadiness(const Game &game) {
            if (!p1.ready && game.getScore() >= p1.pointsThreshold && p1.fixedSinceUse == -1) p1.ready = true;
            if (!p2.ready && game.getScore() >= p2.pointsThreshold && p2.fixedSinceUse == -1) p2.ready = true;
            if (!p3.ready && game.getScore() >= p3.pointsThreshold && p3.fixedSinceUse == -1) p3.ready = true;
            if (!p4.ready && game.getScore() >= p4.pointsThreshold && p4.fixedSinceUse == -1) p4.ready = true;
        }

        void attemptActivate(Game &game, int idx) {
            switch (idx) {
                case 0:
                    if (p1.ready) {
                        game.fillBottomHole();
                        p1.ready = false;
                        p1.fixedSinceUse = 0;
                    }
                    break;
                case 1:
                    if (p2.ready) {
                        game.skipCurrentPiece();
                        p2.ready = false;
                        p2.fixedSinceUse = 0;
                    }
                    break;
                case 2:
                    if (p3.ready) {
                        game.applySlowToActivePiece(3);
                        p3.ready = false;
                        p3.fixedSinceUse = 0;
                    }
                    break;
                case 3:
                    if (p4.ready) {
                        game.deleteTopRows(3);
                        p4.ready = false;
                        p4.fixedSinceUse = 0;
                    }
                    break;
                default:
                    break;
            }
        }
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
