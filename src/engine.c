#include <SDL3/SDL.h>
#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Input/joystick.h>
#include <Supergoon/Input/keyboard.h>
#include <Supergoon/clock.h>
#include <Supergoon/engine.h>
#include <Supergoon/events.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <Supergoon/window.h>
#include <SupergoonEngine/Animation/animator.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/graphics.h>
#include <SupergoonEngine/map.h>
#include <SupergoonEngine/sprite.h>
#include <SupergoonEngine/ui.h>

// This is not needed, just for testing
#include <Supergoon/graphics.h>
#ifdef imgui
#include <Supergoon/Debug/ImGui.hpp>
#endif

// this is not needed, and should be consolidated to one function
#include <SupergoonEngine/Lua/animation.h>
#include <SupergoonEngine/Lua/audio.h>
#include <SupergoonEngine/Lua/effects.h>
#include <SupergoonEngine/Lua/engine.h>
#include <SupergoonEngine/Lua/input.h>
#include <SupergoonEngine/Lua/log.h>
#include <SupergoonEngine/Lua/object.h>
#include <SupergoonEngine/Lua/scene.h>
#include <SupergoonEngine/Lua/sprite.h>
#include <SupergoonEngine/Lua/ui.h>
// Test cap frame rate
#include <SupergoonEngine/window.h>
//

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
Uint64 previousCounter;
Uint64 _frequency;
float deltaTimeSeconds;

// static geClock _clock;
static void (*_startFunc)(void) = NULL;
static void (*_updateFunc)(void) = NULL;
static void (*_drawFunc)(void) = NULL;
static int (*_handleEventFunc)(Event *) = NULL;
#ifdef imgui
bool _isGameSimulatorRunning = true;
#endif

static bool Start(void) {
	// geClockStart(&_clock);
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
	CreateWindow();
	initializeAudio();
	initializeTweenEngine();
	InitializeGameObjectSystem();
	InitializeSpriteSystem();
	InitializeUISystem();
	// should be consolidated
	RegisterLuaUIFunctions();
	RegisterLuaLogFunctions();
	RegisterLuaAudioFunctions();
	RegisterLuaObjectFunctions();
	RegisterLuaSceneFuncs();
	RegisterLuaEngineFunctions();
	RegisterLuaEffectsFunctions();
	RegisterLuaInputFunctions();
	RegisterLuaSpriteFunctions();
	RegisterLuaAnimationFunctions();
	_frequency = SDL_GetPerformanceFrequency();	 // ticks per second
	previousCounter = SDL_GetPerformanceCounter();
	deltaTimeSeconds = 0;
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
		Uint64 now = SDL_GetPerformanceCounter();
		deltaTimeSeconds = (now - previousCounter) / (float)_frequency;
		previousCounter = now;
		DeltaTimeMilliseconds = deltaTimeSeconds * 1000;
		DeltaTimeSeconds = deltaTimeSeconds;
		quit = sdlEventLoop();
		if (quit) {
			break;
		}
		// #ifdef imgui
		// 		// If we are im imgui and the game is "paused", we should just Draw and update imgui.
		// 		if (!_isGameSimulatorRunning) {
		// 		}
		// #endif

		// Update
		UpdateKeyboardSystem();
		audioUpdate();
		Ticks += 1;
		updateTweens();
		UpdateAnimators();

		PushGamestateToLua();
		GameObjectSystemUpdate();
		if (_updateFunc) _updateFunc();
		UpdateUISystem();
		geUpdateControllerLastFrame();
		updateMouseSystem();
		// Draw
		DrawStart();
		drawCurrentMap();
		DrawSpriteSystem();
		if (_drawFunc) _drawFunc();
		DrawUISystem();
		DrawEnd();
		if (TARGET_FPS != 999) {  // If we are doing a capped frame rate, we should also wait between frames.
			Uint64 frame_end = SDL_GetPerformanceCounter();
			Uint64 elapsed_ticks = frame_end - now;
			Uint64 elapsed_ns = (elapsed_ticks * 1000000000ULL) / _frequency;
			const Uint64 FRAME_DURATION_NS = 1000000000 / TARGET_FPS;
			if (elapsed_ns < FRAME_DURATION_NS) {
				SDL_DelayPrecise(FRAME_DURATION_NS - elapsed_ns);
			}
		}
	}
}

static void Quit(void) {
	shutdownMapSystem();
	ShutdownSpriteSystem();
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
