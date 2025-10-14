#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "App.h"

extern "C" {

SDL_AppResult SDL_AppInit(void ** /*appstate*/, int argc, char **argv) {
    return App::instance().init(argc, argv);
}

SDL_AppResult SDL_AppEvent(void * /*appstate*/, SDL_Event *event) {
    return App::instance().onEvent(event);
}

SDL_AppResult SDL_AppIterate(void * /*appstate*/) {
    return App::instance().iterate();
}

void SDL_AppQuit(void * /*appstate*/, SDL_AppResult result) {
    App::instance().quit(result);
}

} // extern "C"

