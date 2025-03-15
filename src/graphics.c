#include <SDL3/SDL.h>
#include <Supergoon/window.h>
#include <assert.h>
#include <stdlib.h>
void DrawRect(void) {
	Renderer* renderer = GetGameRenderer();
	assert(renderer && "Could not get game renderer for some reason");
	SDL_FRect rect = {0, 0, 16, 16};
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
