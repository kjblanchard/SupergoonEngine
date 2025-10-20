#include <SDL3/SDL.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/Platform/sdl/sdl.h>
int InitializeSdl(void) {
	int options = 0;
	options |= SDL_INIT_VIDEO | SDL_INIT_GAMEPAD;
	if (!SDL_Init(options)) {
		sgLogError("Could not init sdl, %s", SDL_GetError());
		return false;
	}
}
