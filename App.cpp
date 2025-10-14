#include "App.h"
#include "DiagnosticHelper.h"

#include <iostream>
#include <vector>
#include <iomanip>

// Singleton access
App& App::instance() {
    static App inst;
    return inst;
}

// Cleanup SDL resources
App::~App() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

// Initialize SDL, create window/renderer, and print diagnostic info
SDL_AppResult App::init(int /*argc*/, char** /*argv*/) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    DiagnosticHelper::printSDLRenderDrivers();

    if (!SDL_CreateWindowAndRenderer("CellularCluster3", WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL, &window_, &renderer_)) {
        SDL_Log("SDL_CreateWindowAndRenderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer_, WIDTH, HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH);

    DiagnosticHelper::printSDLRendererInfo(renderer_);
    DiagnosticHelper::printOpenCLInfo();

    return SDL_APP_CONTINUE;
}

// Handle keyboard and window events
SDL_AppResult App::onEvent(const SDL_Event* event) {
    if (!event) return SDL_APP_CONTINUE;

    if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
    if (event->type == SDL_EVENT_KEY_DOWN) {
        // Press ESC to quit
        if (event->key.key == SDLK_ESCAPE) return SDL_APP_SUCCESS;
        // Toggle fullscreen with F key
        if (event->key.key == SDLK_F) {
            const bool is_fullscreen = SDL_GetWindowFlags(window_) & SDL_WINDOW_FULLSCREEN;
            SDL_SetWindowFullscreen(window_, !is_fullscreen);
        }
    }

    return SDL_APP_CONTINUE;
}

// Main rendering loop
SDL_AppResult App::loop() {

    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);

    return SDL_APP_CONTINUE;
}

// Called before shutdown
void App::quit(SDL_AppResult /*result*/) {
    // Destructor will handle cleanup
}


