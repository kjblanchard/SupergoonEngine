#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
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

bool HandleSDLEvents(void* e) {
	SDL_Event* event = (SDL_Event*)e;
	switch (event->type) {
		case SDL_EVENT_QUIT:
			sgLogWarn("Going to quit from engine");
			return true;
		case SDL_EVENT_KEY_DOWN:
			if (event->key.key == SDLK_Q) return true;
			break;
		default:
			break;
	}
	return false;
}
