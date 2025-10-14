#pragma once

#include <SDL3/SDL.h>

class FPSCounter {
public:
    FPSCounter() = default;

    void update();
    void render(SDL_Renderer* renderer);
    void toggle();

    bool isShowing() const { return showFPS_; }
    double getFPS() const { return currentFPS_; }

private:
    bool showFPS_ {false};
    Uint64 frameCount_ {0};
    Uint64 lastFPSTime_ {0};
    double currentFPS_ {0.0};
};
