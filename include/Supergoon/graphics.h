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
#include <Supergoon/Primitives/rectangle.h>
typedef struct SDL_Texture Texture;

/**
 * @brief Draws a rectangle on the screen, currently doesn't take any params hah..
 *
 */
void DrawRect(RectangleF* rect, sgColor* color, int filled);
/**
 * @brief Draws a texture to the screen, make sure we are in a drawstart.
 * @param texture
 * @param dst
 * @param src
 */
void DrawTexture(Texture* texture, RectangleF* dst, RectangleF* src);
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

void ClearRenderTargetTexture(Texture* texture, sgColor* color);

/**
 * @brief Draws a texture on a render target texture.  Useful for creating something that you can draw with, Used by engine for bg1 and bg2
 * Make sure dst was created with CreateRenderTargetTexture
 *
 * @param dst
 * @param src
 * @param dstRect
 * @param srcRect
 */

void DrawTextureToRenderTargetTexture(Texture* dst, Texture* src, RectangleF* dstRect, RectangleF* srcRect);

/**
 * @brief Loads a bmp from the assets/img folder
 *
 * @param filename The name of the file, without the .bmp
 * @return Texture* Loaded texture, or null if failure
 */
Texture* CreateTextureFromIndexedBMP(const char* filename);

void UnloadTexture(Texture* texture);
void UnloadUnusedTextures(void);
void UnloadAllTextures(void);
