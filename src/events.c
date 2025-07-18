#include <SDL3/SDL_events.h>
#include <Supergoon/Audio/Audio.h>
#include <Supergoon/events.h>
#include <stdbool.h>

// Function in audio.c
extern void audioEventHandler(Event *event);
// Function in tween.c
extern void handleTweenEvents(Event *event);
// Functions in mouce.c
extern void handleMouseEvent(const SDL_Event *event);
extern void handleTouchEvent(const SDL_Event *event);
extern void windowEventHandler(Event *event);
BuiltinEventTypes BuiltinEventIds;
static int (*_customEventHandler)(Event *event) = NULL;

void InitializeEventEngine(void) {
	BuiltinEventIds.LoadBgmEvent = SDL_RegisterEvents(1);
	BuiltinEventIds.PlayBgmEvent = SDL_RegisterEvents(1);
	BuiltinEventIds.StopBgmEvent = SDL_RegisterEvents(1);
	BuiltinEventIds.PauseBgmEvent = SDL_RegisterEvents(1);
	BuiltinEventIds.StartTweenEvent = SDL_RegisterEvents(1);
	BuiltinEventIds.PauseTweenEvent = SDL_RegisterEvents(1);
	BuiltinEventIds.StopTweenEvent = SDL_RegisterEvents(1);
}

void PushEvent(uint32_t eventType, int eventCode, void *data, void *data2) {
	SDL_Event event;
	SDL_zero(event);
	event.type = eventType;
	event.user.code = eventCode;
	event.user.data1 = data;
	event.user.data2 = data2;
	SDL_PushEvent(&event);
}

int HandleEvents(Event *event) {
	audioEventHandler(event);
	// handleTweenEvents(event);
	handleMouseEvent(event);
	handleTouchEvent(event);
	windowEventHandler(event);
	return false;
}

void SetCustomEventHandler(int (*eventHandlerFunction)(Event *event)) {
	_customEventHandler = eventHandlerFunction;
}

int HandleCustomEventHandler(Event *event) {
	if (_customEventHandler) {
		return _customEventHandler(event);
	}
	return false;
}
