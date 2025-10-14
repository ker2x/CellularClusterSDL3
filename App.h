#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <CL/cl.h>
#include <string>

class App {
public:
    static App& instance();

    SDL_AppResult init(int argc, char** argv);
    SDL_AppResult onEvent(SDL_Event* event);
    SDL_AppResult iterate();
    void quit(SDL_AppResult result);

    // Non-copyable, non-movable
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

private:
    App() = default;
    ~App();

    // Helpers
    void printSDLRenderDrivers() const;
    void printSDLRendererInfo(SDL_Renderer* r) const;
    void printOpenCLInfo() const;

    // State
    SDL_Window* window_ {nullptr};
    SDL_Renderer* renderer_ {nullptr};

    static constexpr int WIDTH  = 640;
    static constexpr int HEIGHT = 480;
};
