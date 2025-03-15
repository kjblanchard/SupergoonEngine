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
typedef struct SDL_Window Window;
typedef struct SDL_Renderer Renderer;

/**
 * @brief Creates the Game Window and renderer
 *
 * @param width
 * @param height
 * @param name
 */
void CreateWindow(int width, int height, const char* name);
Window* GetGameWindow(void);
Renderer* GetGameRenderer(void);
void DrawStart(void);
void DrawEnd(void);
