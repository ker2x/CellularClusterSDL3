#include "FPSCounter.h"
#include <cstdio>
#include <string>

void FPSCounter::update() {
    frameCount_++;
    Uint64 currentTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();

    if (lastFPSTime_ == 0) {
        lastFPSTime_ = currentTime;
    }

    double elapsed = static_cast<double>(currentTime - lastFPSTime_) / frequency;
    if (elapsed >= 1.0) {
        currentFPS_ = frameCount_ / elapsed;
        frameCount_ = 0;
        lastFPSTime_ = currentTime;
    }
}

void FPSCounter::toggle() {
    showFPS_ = !showFPS_;
}

void FPSCounter::render(SDL_Renderer* renderer) {
    if (!showFPS_ || !renderer) return;

    // Simple 7-segment style digit patterns (1=on, 0=off for each segment)
    // Segments: top, top-right, bottom-right, bottom, bottom-left, top-left, middle
    static constexpr bool digits[10][7] = {
        {1,1,1,1,1,1,0}, // 0
        {0,1,1,0,0,0,0}, // 1
        {1,1,0,1,1,0,1}, // 2
        {1,1,1,1,0,0,1}, // 3
        {0,1,1,0,0,1,1}, // 4
        {1,0,1,1,0,1,1}, // 5
        {1,0,1,1,1,1,1}, // 6
        {1,1,1,0,0,0,0}, // 7
        {1,1,1,1,1,1,1}, // 8
        {1,1,1,1,0,1,1}  // 9
    };

    const int digitWidth = 12;
    const int digitHeight = 20;
    const int segmentThickness = 2;
    const int spacing = 4;
    int x = 10;
    const int y = 10;

    // Convert FPS to string
    char buf[32];
    snprintf(buf, sizeof(buf), "FPS:%d", static_cast<int>(currentFPS_ + 0.5));
    std::string fpsText(buf);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    for (char c : fpsText) {
        if (c >= '0' && c <= '9') {
            int digit = c - '0';
            const bool* segs = digits[digit];

            // Top
            if (segs[0]) {
                SDL_FRect rect = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(digitWidth), static_cast<float>(segmentThickness)};
                SDL_RenderFillRect(renderer, &rect);
            }
            // Top-right
            if (segs[1]) {
                SDL_FRect rect = {static_cast<float>(x + digitWidth - segmentThickness), static_cast<float>(y), static_cast<float>(segmentThickness), static_cast<float>(digitHeight / 2)};
                SDL_RenderFillRect(renderer, &rect);
            }
            // Bottom-right
            if (segs[2]) {
                SDL_FRect rect = {static_cast<float>(x + digitWidth - segmentThickness), static_cast<float>(y + digitHeight / 2), static_cast<float>(segmentThickness), static_cast<float>(digitHeight / 2)};
                SDL_RenderFillRect(renderer, &rect);
            }
            // Bottom
            if (segs[3]) {
                SDL_FRect rect = {static_cast<float>(x), static_cast<float>(y + digitHeight - segmentThickness), static_cast<float>(digitWidth), static_cast<float>(segmentThickness)};
                SDL_RenderFillRect(renderer, &rect);
            }
            // Bottom-left
            if (segs[4]) {
                SDL_FRect rect = {static_cast<float>(x), static_cast<float>(y + digitHeight / 2), static_cast<float>(segmentThickness), static_cast<float>(digitHeight / 2)};
                SDL_RenderFillRect(renderer, &rect);
            }
            // Top-left
            if (segs[5]) {
                SDL_FRect rect = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(segmentThickness), static_cast<float>(digitHeight / 2)};
                SDL_RenderFillRect(renderer, &rect);
            }
            // Middle
            if (segs[6]) {
                SDL_FRect rect = {static_cast<float>(x), static_cast<float>(y + digitHeight / 2 - segmentThickness / 2), static_cast<float>(digitWidth), static_cast<float>(segmentThickness)};
                SDL_RenderFillRect(renderer, &rect);
            }

            x += digitWidth + spacing;
        } else if (c == ':') {
            // Draw colon as two small rectangles
            SDL_FRect rect1 = {static_cast<float>(x + 2), static_cast<float>(y + digitHeight / 3), 2.0f, 2.0f};
            SDL_FRect rect2 = {static_cast<float>(x + 2), static_cast<float>(y + 2 * digitHeight / 3), 2.0f, 2.0f};
            SDL_RenderFillRect(renderer, &rect1);
            SDL_RenderFillRect(renderer, &rect2);
            x += 6 + spacing;
        } else {
            // Space for other characters (like 'F', 'P', 'S')
            x += 8;
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
