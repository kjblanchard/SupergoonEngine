#include <SDL3/SDL.h>
#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Input/joystick.h>
#include <Supergoon/Input/keyboard.h>
#include <Supergoon/clock.h>
#include <Supergoon/engine.h>
#include <Supergoon/events.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/state.h>
#include <Supergoon/window.h>
#include <SupergoonEngine/map.h>
// This is not needed, just for testing
#include <Supergoon/graphics.h>
#ifdef imgui
#include <Supergoon/Debug/ImGui.hpp>
#endif

// Functions in Audio.c
extern void initializeAudio(void);
extern void closeAudio(void);
extern void audioUpdate(void);
// Function in tween.c
extern void initializeTweenEngine(void);
extern void updateTweens(void);
// Texting, in map.c
extern Texture *bg1Texture;

static geClock _clock;
static void (*_startFunc)(void) = NULL;
static void (*_updateFunc)(void) = NULL;
static void (*_drawFunc)(void) = NULL;
static int (*_handleEventFunc)(Event *) = NULL;

static bool Start(void) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
		sgLogError("Could not init sdl, %s", SDL_GetError());
		return false;
	}
	sgInitializeDebugLogFile();
	InitializeKeyboardSystem();
	geInitializeJoysticks();
	InitializeLuaEngine();
	InitializeEventEngine();
	geClockStart(&_clock);
	CreateWindow();
	initializeAudio();
	initializeTweenEngine();
	Tilemap *map = parseTiledTilemap("debugTown");
	createBackgroundsFromTilemap(map);
	freeTiledTilemap(map);

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
#ifdef imgui
		if (!HandleImGuiEvent(&event)) {
			sgLogWarn("We are not sending this to the game");
			continue;
		}
		sgLogWarn("We are sending this to the game");
#endif
		if (_handleEventFunc) quit = _handleEventFunc(&event);
		HandleEvents(&event);
	}
	return quit;
}

static void Update(void) {
	bool quit = false;
	while (!quit) {
		quit = sdlEventLoop();
		UpdateKeyboardSystem();
		geClockUpdate(&_clock);
		DeltaTimeMilliseconds = geClockGetUpdateTimeMilliseconds();
		DeltaTimeSeconds = geClockGetUpdateTimeSeconds();
		Ticks += 1;
		audioUpdate();
		updateTweens();
		if (_updateFunc) _updateFunc();
		DrawStart();
		if (bg1Texture) {
			Rectangle src = {0, 0, 512, 288};
			DrawTexture(bg1Texture, &src, &src);
		}
		// DrawRect();
		if (_drawFunc) _drawFunc();
		DrawEnd();
		geUpdateControllerLastFrame();
	}
}

static void Quit(void) {
	sgCloseDebugLogFile();
	ShutdownJoystickSystem();
	sgCloseLua();
	closeAudio();
	CloseWindow();
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
