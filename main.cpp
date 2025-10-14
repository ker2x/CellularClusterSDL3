#define SDL_MAIN_USE_CALLBACKS 1

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window;
static SDL_Renderer *renderer;

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;
constexpr int N = 100;

// init SDL
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_CreateWindowAndRenderer("CellularCluster3", 640, 480, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_STRETCH);
    return SDL_APP_CONTINUE;
}

// handle events
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    // Quit if application quit
    if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
    // Quit is ESC is pressed
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE) return SDL_APP_SUCCESS;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}

