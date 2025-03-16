/**
 * @file graphics.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief  Creating Textures and Drawing functions
 * @version 0.1
 * @date 2025-03-14
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <Supergoon/Primitives/Color.h>
typedef struct SDL_Texture Texture;

/**
 * @brief Draws a rectangle on the screen, currently doesn't take any params hah..
 *
 */
void DrawRect(void);
/**
 * @brief Creates a Texture that can be used as a render target.
 * Used if this will be drawn on by other draw calls
 *
 * @param width
 * @param height
 * @param color color that this texture will be by default
 * @return Texture*
 */
Texture* CreateRenderTargetTexture(int width, int height, sgColor color);
