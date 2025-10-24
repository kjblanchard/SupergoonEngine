#include <Supergoon/Audio/Audio.h>
#include <Supergoon/events.h>
#include <stdbool.h>
#include <stdlib.h>

// Function in tween.c
extern void handleTweenEvents(void *event);
// Functions in mouce.c
extern void handleMouseEvent(void *event);
// extern void handleTouchEvent(void *event);
extern void windowEventHandler(void *event);
BuiltinEventTypes BuiltinEventIds;
static int (*_customEventHandler)(void *event) = NULL;
static int _currentCustomRegisteredEvent = 1000;

static int _shouldQuit = 0;

void InitializeEventSystem(void) {
	BuiltinEventIds.QuitGameEvent = _currentCustomRegisteredEvent++;
	BuiltinEventIds.LoadBgmEvent = _currentCustomRegisteredEvent++;
	BuiltinEventIds.PlayBgmEvent = _currentCustomRegisteredEvent++;
	BuiltinEventIds.StopBgmEvent = _currentCustomRegisteredEvent++;
	BuiltinEventIds.PauseBgmEvent = _currentCustomRegisteredEvent++;
	BuiltinEventIds.StartTweenEvent = _currentCustomRegisteredEvent++;
	BuiltinEventIds.PauseTweenEvent = _currentCustomRegisteredEvent++;
	BuiltinEventIds.StopTweenEvent = _currentCustomRegisteredEvent++;
}

void PushEvent(uint32_t eventType, int eventCode, void *data, void *data2) {
	// SDL_Event event;
	// SDL_zero(event);
	// event.type = eventType;
	// event.user.code = eventCode;
	// event.user.data1 = data;
	// event.user.data2 = data2;
	// SDL_PushEvent(&event);
	if (eventType == BuiltinEventIds.QuitGameEvent) _shouldQuit = true;
}

int HandleEvents(void *event) {
	AudioEventHandler(event);
// handleTweenEvents(event);
#ifndef tui
	handleMouseEvent(event);
	// handleTouchEvent(event);
	windowEventHandler(event);
#endif
	return _shouldQuit;
	return false;
}

void SetCustomEventHandler(int (*eventHandlerFunction)(void *event)) {
	_customEventHandler = eventHandlerFunction;
}

int HandleCustomEventHandler(void *event) {
	if (_customEventHandler) {
		return _customEventHandler(event);
	}
	return false;
}
