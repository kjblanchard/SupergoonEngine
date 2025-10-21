#include <SDL3/SDL.h>
#include <Supergoon/Platform/sdl/sdl.h>
#include <Supergoon/log.h>
int InitializeSdl(void) {
	int options = 0;
	options |= SDL_INIT_VIDEO | SDL_INIT_GAMEPAD;
	if (!SDL_Init(options)) {
		sgLogError("Could not init sdl, %s", SDL_GetError());
		return false;
	}
	return true;
}
