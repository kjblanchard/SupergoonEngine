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
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>

/**
 * @brief Draws a rectangle on the screen, currently doesn't take any params
 * hah..
 *
 */
void DrawRect(RectangleF *rect, sgColor *color, int filled);

void UnloadTexture(Texture *texture);
void UnloadUnusedTextures(void);
void UnloadAllTextures(void);

// struct SDL_Surface;

// typedef struct TextureCacheItem {
// 	Texture* Texture;
// 	char* name;
// 	uint16_t References;

// } TextureCacheItem;
// extern TextureCacheItem* _textureCache;
// extern size_t _firstCacheHole;
// extern bool _holes;
// extern size_t _numTexturesInCache;
void InitializeGraphicsSystem(void);
// Texture* loadTextureFromSurface(struct SDL_Surface* surface);
void ShutdownGraphicsSystem(void);

void DrawStart(void);
void DrawEnd(void);
void GraphicsSetLogicalWorldSize(int width, int height);
int GraphicsGetTargetRefreshRate(void);
void GraphicsWindowResizeEvent(int width, int height);
