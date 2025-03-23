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
typedef struct SDL_Renderer Renderer;
typedef struct SDL_Texture Texture;

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
 * to the windows width and height.  Does not affect if ImGui enabled
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
#ifdef __cplusplus
}
#endif
