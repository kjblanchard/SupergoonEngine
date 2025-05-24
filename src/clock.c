#include <SDL3/SDL_timer.h>
#include <Supergoon/clock.h>
#include <SupergoonEngine/window.h>

static double _fixedTimestepSeconds = 0;

void geClockStart(geClock* c) {
	c->Previous = SDL_GetPerformanceCounter();
	c->Accumulator = 0;
}

void geClockUpdate(geClock* c) {
	uint64_t current = SDL_GetPerformanceCounter();
	double frameTime = (double)(current - c->Previous) / SDL_GetPerformanceFrequency();
	if (frameTime > 0.25) frameTime = 0.25;	 // Avoid spiral of death
	c->Previous = current;
	c->Accumulator += frameTime;
}

bool geClockShouldUpdate(geClock* c) {
	_fixedTimestepSeconds = 1.0 / (double)_refreshRate;
	if (c->Accumulator >= _fixedTimestepSeconds) {
		c->Accumulator -= _fixedTimestepSeconds;
		return true;
	}
	return false;
}
double geClockGetUpdateTimeMilliseconds(void) {
	return _fixedTimestepSeconds * 1000;
}
double geClockGetUpdateTimeSeconds(void) {
	return _fixedTimestepSeconds;
}
