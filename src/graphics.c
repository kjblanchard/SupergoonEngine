#include <SDL3/SDL.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/graphics.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
#include <assert.h>
#include <stdlib.h>

// defined in window.c
extern Window* _window;
extern Renderer* _renderer;

void DrawRect(void) {
	assert(_renderer && "No renderer, make sure window is created");
	SDL_FRect rect = {0, 0, 16, 16};
	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
	SDL_RenderRect(_renderer, &rect);
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
}

Texture* CreateRenderTargetTexture(int width, int height, sgColor color) {
	assert(_renderer && "No renderer, make sure window is created");
	Texture* image = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	if (!SDL_SetRenderTarget(_renderer, image)) {
		sgLogError("Error setting render target when creating render target texture, %s", SDL_GetError());
	}
	if (!SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND)) {
		sgLogError("Error setting blend mode: %s", SDL_GetError());
	}
	SDL_SetTextureScaleMode(image, SDL_SCALEMODE_NEAREST);
	SDL_SetTextureBlendMode(image, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(_renderer, color.R, color.G, color.B, color.A);
	SDL_RenderClear(_renderer);
	SDL_SetRenderTarget(_renderer, NULL);
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	return image;
}

Texture* CreateTextureFromIndexedBMP(const char* filename) {
	char name[50];
	char fullFilepath[200];
	snprintf(name, sizeof(name), "assets/img/%s.bmp", filename);
	GetFilenameWithExeFilepath(fullFilepath, sizeof(fullFilepath), name);
	SDL_Surface* surface = SDL_LoadBMP(fullFilepath);
	if (!surface) {
		sgLogError("Could not load bmp into surface, %s %s", fullFilepath, SDL_GetError());
		return NULL;
	}
	SDL_Color transparentColor = {255, 0, 255, 255};
	if (!SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, transparentColor.r, transparentColor.g, transparentColor.b))) {
		printf("Failed to set color key: %s\n", SDL_GetError());
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
	if (!texture) {
		sgLogError("Could not load texture from surface, %s: %s", fullFilepath, SDL_GetError());
	}
	return texture;
}
