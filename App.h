#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <CL/cl.h>
#include <string>

/**
 * Main application class using SDL3 callback-based architecture and OpenCL.
 * Singleton pattern for integration with SDL3's C-style callbacks.
 */
class App {
public:
    /// Get the singleton instance
    static App& instance();

    /// Initialize SDL and OpenCL systems
    SDL_AppResult init(int argc, char** argv);

    /// Handle SDL events (keyboard, window, etc.)
    SDL_AppResult onEvent(const SDL_Event* event);

    /// Main render/update loop
    SDL_AppResult loop();

    /// Cleanup and shutdown
    void quit(SDL_AppResult result);

    // Non-copyable, non-movable
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

private:
    App() = default;
    ~App();

    // State
    SDL_Window* window_ {nullptr};
    SDL_Renderer* renderer_ {nullptr};

    static constexpr int WIDTH  = 640;
    static constexpr int HEIGHT = 480;
};
