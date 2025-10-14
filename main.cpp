/**
 * @file main.cpp
 * @brief Entry point for the Cellular Cluster SDL3 application using SDL3's callback-based main loop.
 *
 * This file implements SDL3's callback-based application lifecycle by defining the required
 * callback functions (SDL_AppInit, SDL_AppEvent, SDL_AppIterate, SDL_AppQuit) instead of
 * providing a traditional main() function. All callbacks delegate to the singleton App instance.
 */
#define SDL_MAIN_USE_CALLBACKS = 1

#include <SDL3/SDL_main.h>
#include "App.h"

extern "C" {

/**
 * @brief Initializes the application.
 *
 * Called once at application startup by SDL3's main loop. This function delegates
 * initialization to the singleton App instance.
 *
 * @param appstate application state pointer (reserved for future use, not used in this implementation)
 * @param argc Command line argument count
 * @param argv Command line argument values
 * @return SDL_AppResult indicating success or failure (SDL_APP_CONTINUE or SDL_APP_FAILURE)
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    return App::instance().init(argc, argv);
}

/**
 * @brief Handles SDL events.
 *
 * Called by SDL3's main loop when an event occurs (e.g., keyboard input, mouse movement,
 * window events). This function delegates event handling to the singleton App instance.
 *
 * @param appstate Application state pointer (not used in this implementation)
 * @param event Pointer to the SDL_Event to be processed
 * @return SDL_AppResult indicating whether to continue or quit (SDL_APP_CONTINUE or SDL_APP_SUCCESS)
 */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    return App::instance().onEvent(event);
}

/**
 * @brief Performs one iteration of the main loop.
 *
 * Called continuously by SDL3's main loop to update and render the application.
 * This function delegates the iteration logic to the singleton App instance.
 *
 * @param appstate Application state pointer (not used in this implementation)
 * @return SDL_AppResult indicating whether to continue or quit (SDL_APP_CONTINUE or SDL_APP_SUCCESS)
 */
SDL_AppResult SDL_AppIterate(void *appstate) {
    return App::instance().loop();
}

/**
 * @brief Cleans up and quits the application.
 *
 * Called once when the application is terminating. This function delegates cleanup
 * to the singleton App instance.
 *
 * @param appstate Application state pointer (not used in this implementation)
 * @param result The result code indicating why the application is quitting
 */
void SDL_AppQuit(void * /*appstate*/, SDL_AppResult result) {
    App::instance().quit(result);
}

} // extern "C"

