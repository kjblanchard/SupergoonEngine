/**
 * @file window.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief Window initialization and helper functions
 * @version 0.1
 * @date 2025-03-14
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_Window Window;

/**
 * @brief Set the Window Options object
 *
 * @param width size of window
 * @param height height of window
 * @param name window name
 */
void SetWindowOptions(int width, int height, const char* name);
/**
 * @brief Creates the Game Window and renderer, called by the engine on startup,
 * make sure to call SetWindowOptions before run
 */
void CreateWindow(void);
int WindowHeight(void);
int WindowWidth(void);
void CloseWindow(void);
typedef struct SDL_Window Window;
#ifdef __cplusplus
extern "C" {
#endif
extern Window* _window;
// extern int _refreshRate;
// extern int _vsyncEnabled;
extern int TARGET_FPS;

int getRefreshRate(void);
void handleWindowEvents();
#ifdef __cplusplus
}
#endif
