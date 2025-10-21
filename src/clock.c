#include <Supergoon/clock.h>
#include <Supergoon/tools.h>
#include <Supergoon/window.h>
#include <stdbool.h>

static double _fixedTimestepSeconds = 0;

void geClockStart(geClock* c) {
	c->Previous = getCurrentMSTicks();
	c->Accumulator = 0;
}

void geClockUpdate(geClock* c) {
	uint64_t current = getCurrentMSTicks();
	double frameTime = (current - c->Previous);
	if (frameTime > 0.25) frameTime = 0.25;	 // Avoid spiral of death
	c->Previous = current;
	c->Accumulator += frameTime;
	_fixedTimestepSeconds = 1.0 / (double)_refreshRate;
}

int geClockShouldUpdate(geClock* c) {
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
