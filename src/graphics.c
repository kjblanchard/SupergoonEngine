#include <SDL3/SDL.h>
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
