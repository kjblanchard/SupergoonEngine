/**
 * @file graphics.h
 * @author Kevin Blanchard (kevin@supergoon.com)
 * @brief Graphics file for engine, declarations mostly used in imgui and in other places non-public
 * @version 0.1
 * @date 2025-05-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <Supergoon/graphics.h>
#include <stdbool.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

struct SDL_Surface;

typedef struct TextureCacheItem {
	Texture* Texture;
	char* name;
	uint16_t References;

} TextureCacheItem;
extern TextureCacheItem* _textureCache;
extern size_t _firstCacheHole;
extern bool _holes;
extern size_t _numTexturesInCache;
void initializeGraphicsSystem(void);
Texture* loadTextureFromSurface(struct SDL_Surface* surface);
void shutdownGraphicsSystem(void);
#ifdef __cplusplus
}
#endif
