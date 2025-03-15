#include <SDL3/SDL_timer.h>
#include <Supergoon/clock.h>

static double _deltaTimeInSeconds;


void geClockStart(geClock* c) {
	c->Previous = SDL_GetPerformanceCounter();
}

void geClockUpdate(geClock* c) {
	uint64_t current = SDL_GetPerformanceCounter();
	_deltaTimeInSeconds = (double)(current - c->Previous) / (double)SDL_GetPerformanceFrequency();
	c->Previous = current;
}

double geClockGetUpdateTimeMilliseconds(void) {
	return _deltaTimeInSeconds * 1000;
}

double geClockGetUpdateTimeSeconds(void) {
	return _deltaTimeInSeconds;
}
