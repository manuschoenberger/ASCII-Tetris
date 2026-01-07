#pragma once

#include <memory>
#include <string>

class Game; // forward

class IMode {
public:
    virtual ~IMode() = default;
    virtual void onStart(Game &game) {}
    virtual void onTick(Game &game, int tick) {}
    virtual void onInput(Game &game, int key) {}
    virtual std::string name() const = 0;
};

// simple factory helpers
std::shared_ptr<IMode> createNormalMode();
std::shared_ptr<IMode> createFunMode();
std::shared_ptr<IMode> createHardMode();
std::shared_ptr<IMode> createMixedMode();

