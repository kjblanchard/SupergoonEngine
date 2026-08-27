#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <Supergoon/Platform/sdl/sdl.h>
#include <sgtools/log.h>
int InitializeSdl(void) {
	int options = 0;
	SDL_SetHint(SDL_HINT_MAC_BACKGROUND_APP, "0");
	options |= SDL_INIT_VIDEO | SDL_INIT_GAMEPAD;
	sgLogWarn("[SDL] Initializing SDL with flags: 0x%X", options);
	if (!SDL_Init(options)) {
		sgLogError("Could not init sdl, %s", SDL_GetError());
		return false;
	}
	sgLogWarn("[SDL] SDL_Init success");
	return true;
}

bool HandleSDLEvents(void* e) {
	SDL_Event* event = (SDL_Event*)e;
	switch (event->type) {
		case SDL_EVENT_QUIT:
			sgLogWarn("Going to quit from engine");
			return true;
		case SDL_EVENT_KEY_DOWN:
			if (event->key.key == SDLK_Q && !SDL_TextInputActive(SDL_GetKeyboardFocus())) return true;
			break;
		default:
			break;
	}
	return false;
}
