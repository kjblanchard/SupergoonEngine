#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Input/joystick.h>
#include <Supergoon/Input/keyboard.h>
#include <Supergoon/clock.h>
#include <Supergoon/engine.h>
#include <Supergoon/events.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/window.h>
// This is not needed, just for testing
#include <Supergoon/graphics.h>
#ifdef imgui
#include <Supergoon/Debug/ImGui.hpp>
#endif

// Functions in Audio.c
extern void initializeAudio(void);
extern void audioUpdate(void);

static geClock _clock;
static void (*_startFunc)(void) = NULL;
static void (*_updateFunc)(void) = NULL;
static void (*_drawFunc)(void) = NULL;
static int (*_handleEventFunc)(Event *) = NULL;

static bool Start(void) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
		return false;
	}
	sgInitializeDebugLogFile();
	geInitializeKeyboard();
	geInitializeJoysticks();
	sgInitializeLua();
	InitializeEventEngine();
	geClockStart(&_clock);
	CreateWindow();
	initializeAudio();
	return true;
}
static bool sdlEventLoop(void) {
	static SDL_Event event;
	bool quit = false;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				return true;
				break;
		}
		if (_handleEventFunc) quit = _handleEventFunc(&event);
		HandleEvents(&event);
#ifdef imgui
		HandleImGuiEvent(&event);

#endif
	}
	return quit;
}

static void Update(void) {
	bool quit = false;
	while (!quit) {
		quit = sdlEventLoop();
		geUpdateKeyboard();
		geClockUpdate(&_clock);
		audioUpdate();
		if (_updateFunc) _updateFunc();
		DrawStart();
		DrawRect();
		if (_drawFunc) _drawFunc();
		DrawEnd();
		geUpdateControllerLastFrame();
	}
}

static void Quit(void) {
	sgCloseDebugLogFile();
}

void SetStartFunction(void (*startFunc)(void)) {
	_startFunc = startFunc;
}

void SetHandleEventFunction(int (*eventFunc)(Event *)) {
	_handleEventFunc = eventFunc;
}

void SetUpdateFunction(void (*updateFunc)(void)) {
	_updateFunc = updateFunc;
}

void SetDrawFunction(void (*drawFunc)(void)) {
	_drawFunc = drawFunc;
}

void Run(void) {
	bool started = Start();
	if (!started) {
		fprintf(stderr, "Could not start program, exiting");
	}
	Update();
	Quit();
}
