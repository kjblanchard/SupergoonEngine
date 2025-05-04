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
extern bool _holes;
extern size_t _numTexturesInCache;

Texture* LoadTextureFromSurface(struct SDL_Surface* surface);
void initializeGraphicsSystem(void);
void shutdownGraphicsSystem(void);

#ifdef __cplusplus
}
#endif
