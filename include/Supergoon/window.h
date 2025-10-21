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
#include <Supergoon/graphics.h>

typedef struct SDL_Window Window;
typedef struct SDL_Renderer Renderer;

/**
 * @brief Set the Window Options object
 *
 * @param width size of window
 * @param height height of window
 * @param name window name
 */
void SetWindowOptions(int width, int height, const char* name);

/**
 * @brief Setting this will scale the game from the world width and height
 * to the windows width and height.
 *
 * @param worldWidth the size of the world that it should scale from
 * @param worldHeight height of world that it should scale from
 */
void SetScalingOptions(int worldWidth, int worldHeight);
/**
 * @brief Creates the Game Window and renderer, called by the engine on startup,
 * make sure to call SetWindowOptions before run
 */
void CreateWindow(void);
/**
 * @brief Prepares the engine for drawing
 */
void DrawStart(void);
/**
 * @brief Concludes drawing, and draws to screen
 */
void DrawEnd(void);

int WindowHeight(void);
int WindowWidth(void);

void CloseWindow(void);
typedef struct SDL_Renderer Renderer;
typedef struct SDL_Texture Texture;
typedef struct SDL_Window Window;
#ifdef __cplusplus
extern "C" {
#endif

extern Texture* _fullScreenTexture;
extern int _gameImagePosX;
extern int _gameImagePosY;
extern float _gameImageScale;
extern int _gameImageWidth;
extern int _gameImageHeight;
extern Renderer* _renderer;
extern int _logicalWidth;
extern int _logicalHeight;
extern Window* _window;
extern int _refreshRate;
extern int _vsyncEnabled;
extern int TARGET_FPS;

int getRefreshRate(void);
void handleWindowEvents();
#ifdef __cplusplus
}
#endif
