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
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/graphics.h>
#include <SupergoonEngine/map.h>
#include <SupergoonEngine/ui.h>

// This is not needed, just for testing
#include <Supergoon/graphics.h>
#ifdef imgui
#include <Supergoon/Debug/ImGui.hpp>
#endif

// this is not needed, and should be consolidated to one function
#include <SupergoonEngine/Lua/audio.h>
#include <SupergoonEngine/Lua/effects.h>
#include <SupergoonEngine/Lua/engine.h>
#include <SupergoonEngine/Lua/log.h>
#include <SupergoonEngine/Lua/object.h>
#include <SupergoonEngine/Lua/scene.h>
#include <SupergoonEngine/Lua/ui.h>

// Functions in mouce.c
void updateMouseSystem(void);

// Functions in Audio.c
extern void initializeAudio(void);
extern void closeAudio(void);
extern void audioUpdate(void);
// Function in tween.c
extern void initializeTweenEngine(void);
extern void updateTweens(void);
// Function in filesystem.c
extern void shutdownEngineFilesystem(void);

static geClock _clock;
static void (*_startFunc)(void) = NULL;
static void (*_updateFunc)(void) = NULL;
static void (*_drawFunc)(void) = NULL;
static int (*_handleEventFunc)(Event *) = NULL;
#ifdef imgui
bool _isGameSimulatorRunning = true;
#endif

static bool Start(void) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
		sgLogError("Could not init sdl, %s", SDL_GetError());
		return false;
	}
	sgInitializeDebugLogFile();
	InitializeKeyboardSystem();
	initializeGraphicsSystem();
	geInitializeJoysticks();
	InitializeLuaEngine();
	InitializeEventEngine();
	geClockStart(&_clock);
	CreateWindow();
	initializeAudio();
	initializeTweenEngine();
	InitializeGameObjectSystem();
	InitializeUISystem();
	// should be consolidated
	RegisterLuaUIFunctions();
	RegisterLuaLogFunctions();
	RegisterLuaAudioFunctions();
	RegisterLuaObjectFunctions();
	RegisterLuaSceneFuncs();
	RegisterLuaEngineFunctions();
	RegisterLuaEffectsFunctions();
	//
	return true;
}
static bool sdlEventLoop(void) {
	static SDL_Event event;
	bool quit = false;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				return true;
		}
#ifdef imgui
		if (!HandleImGuiEvent(&event)) {
			continue;
		}
#endif
		if (_handleEventFunc) {
			quit = _handleEventFunc(&event);
		}
		HandleEvents(&event);
		// if (_handleEventFunc) quit = _handleEventFunc(&event);
		// HandleEvents(&event);
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
#ifdef imgui
		// If we are im imgui and the game is "paused", we should just Draw and update imgui.
		if (!_isGameSimulatorRunning) {
		}
#endif
		audioUpdate();
		updateTweens();
		GameObjectSystemUpdate();
		if (_updateFunc) _updateFunc();
		UpdateUISystem();
		DrawStart();
		drawCurrentMap();
		if (_drawFunc) _drawFunc();
		DrawUISystem();
		DrawEnd();
		geUpdateControllerLastFrame();
		updateMouseSystem();
	}
}

static void Quit(void) {
	shutdownMapSystem();
	sgCloseDebugLogFile();
	ShutdownJoystickSystem();
	sgCloseLua();
	closeAudio();
	shutdownGraphicsSystem();
	CloseWindow();
	ShutdownUISystem();
	shutdownEngineFilesystem();
	SDL_Quit();
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
	if (_startFunc) _startFunc();
	Update();
	Quit();
}
