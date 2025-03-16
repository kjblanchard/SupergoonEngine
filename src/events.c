#include <Supergoon/events.h>

BuiltinEventTypes BuiltinEventIds;
static int (*_customEventHandler)(Event *event) = NULL;

void InitializeEventEngine(void) {
	BuiltinEventIds.LoadBgmEvent = SDL_RegisterEvents(1);
	BuiltinEventIds.PlayBgmEvent = SDL_RegisterEvents(1);
	BuiltinEventIds.StopBgmEvent = SDL_RegisterEvents(1);
}

int PushEvent(uint32_t eventType, int eventCode, void *data, void *data2) {
	SDL_Event event;
	SDL_zero(event);
	event.type = eventType;
	event.user.code = eventCode;
	event.user.data1 = data;
	event.user.data2 = data2;
	SDL_PushEvent(&event);
}

int HandleEvents(Event *event) {
	(void *)event;
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
