#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Input/keyboard.h>
#include <Supergoon/clock.h>
#include <Supergoon/engine.h>
#include <Supergoon/events.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/state.h>
#include <Supergoon/window.h>
#include <SupergoonEngine/Lua/engine.h>
#include <SupergoonEngine/Lua/scripting.h>
#include <SupergoonEngine/camera.h>
#include <SupergoonEngine/graphics.h>
#include <SupergoonEngine/tools.h>
#ifndef tui
#include <SDL3/SDL.h>
#include <Supergoon/Input/joystick.h>
#include <Supergoon/sprite.h>
#include <SupergoonEngine/Animation/animator.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/map.h>
#include <SupergoonEngine/sprite.h>
#include <SupergoonEngine/ui.h>
#endif
#include <SupergoonEngine/window.h>
#ifdef imgui
#include <Supergoon/Debug/ImGui.hpp>
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static bool eventLoop(void);
// Functions in mouce.c
void updateMouseSystem(void);
void updateTouchSystem(void);
// Function in tween.c
extern void initializeTweenEngine(void);
extern void updateTweens(void);
// Function in filesystem.c
extern void shutdownEngineFilesystem(void);
Uint64 previousCounter;
// Uint64 _frequency;
float deltaTimeSeconds;
static void (*_startFunc)(void) = NULL;
static void (*_updateFunc)(void) = NULL;
static void (*_drawFunc)(void) = NULL;
static void (*_inputFunc)(void) = NULL;
static int (*_handleEventFunc)(void *) = NULL;
#ifdef imgui
bool _isGameSimulatorRunning = true;
#endif
static bool quit = false;
static void Quit(void);

static bool Start(void) {
	sgLogWarn("Starting the things");
	// int options = SDL_INIT_EVENTS;
#ifndef tui
	int options |= SDL_INIT_VIDEO | SDL_INIT_GAMEPAD;
	if (!SDL_Init(options)) {
		sgLogError("Could not init sdl, %s", SDL_GetError());
		return false;
	}
#endif

	sgInitializeDebugLogFile();
	InitializeKeyboardSystem();
#ifndef tui
	initializeGraphicsSystem();
	geInitializeJoysticks();
#endif
	InitializeLuaEngine();
	InitializeEventEngine();
	CreateWindow();
	InitializeAudio();
	initializeTweenEngine();
#ifndef tui
	InitializeUISystem();
#endif
	RegisterAllLuaFunctions();
	// Try to normalize the initial delay of loading everything
	for (size_t i = 0; i < 10; i++) {
		eventLoop();
		sgSleepMS(1);
	}

	// _frequency = SDL_GetPerformanceFrequency();	 // ticks per second
	// previousCounter = SDL_GetPerformanceCounter();
	previousCounter = getCurrentMSTicks();
	deltaTimeSeconds = 0;
	return true;
}
static bool eventLoop(void) {
#ifdef tui
	// TODO we should make some kind of event system.
	return false;
#else
	static SDL_Event event;
	bool quit = false;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				sgLogWarn("Going to quit from engine");
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
	}
	return quit;
#endif
}

static void handleFramerate(Uint64 *now) {
#ifdef __EMSCRIPTEN__
	return;
#elif tui
	uint64_t current = getCurrentMSTicks();
	Uint64 elapsedMS = current - previousCounter;
	const Uint64 FRAME_DURATION_MS = 1000 / TARGET_FPS;
	if (elapsedMS < FRAME_DURATION_MS) {
		sgSleepMS(FRAME_DURATION_MS - elapsedMS);
	}
#else
	if (TARGET_FPS != 999) {  // If we are doing a capped frame rate, we should also wait between frames.
		Uint64 frame_end = SDL_GetPerformanceCounter();
		Uint64 elapsed_ticks = frame_end - *now;
		Uint64 elapsed_ns = (elapsed_ticks * 1000000000ULL) / _frequency;
		const Uint64 FRAME_DURATION_NS = 1000000000 / TARGET_FPS;
		if (elapsed_ns < FRAME_DURATION_NS) {
			SDL_DelayPrecise(FRAME_DURATION_NS - elapsed_ns);
		}
	}
#endif
}

static void draw(void) {
	DrawStart();
#ifndef tui
	drawCurrentMap();
	DrawSpriteSystem();
#endif
	if (_drawFunc) _drawFunc();
#ifndef tui
	DrawUISystem();
#endif
	DrawEnd();
}

static void Update(void) {
	Uint64 now = getCurrentMSTicks();
	DeltaTimeMilliseconds = now - previousCounter;
	previousCounter = now;
	DeltaTimeSeconds = DeltaTimeMilliseconds / 1000;
	quit = eventLoop();
	if (quit) {
#ifdef __EMSCRIPTEN__
		emscripten_cancel_main_loop();
		Quit();
#else
		return;
#endif
	}
	UpdateKeyboardSystem();
	if (_inputFunc) _inputFunc();
#ifndef tui
	UpdateUIInputSystem();
#else

#endif
	Ticks += 1;
	updateTweens();
#ifndef tui
	UpdateAnimators();
#endif
	PushGamestateToLua();
#ifndef tui
	GameObjectSystemUpdate();
#endif
	if (_updateFunc) _updateFunc();
#ifndef tui
	UpdateUISystem();
	geUpdateControllerLastFrame();
	updateMouseSystem();
	updateTouchSystem();
#endif
	UpdateCamera();
	draw();
	handleFramerate(&now);
	AudioUpdate();
}

static void Quit(void) {
#ifndef tui
	shutdownMapSystem();
	ShutdownSpriteSystem();
	ShutdownJoystickSystem();
	shutdownGraphicsSystem();
#endif
	sgCloseDebugLogFile();
	sgCloseLua();
	CloseAudio();
	CloseWindow();
#ifndef tui
	ShutdownUISystem();
#endif
	shutdownEngineFilesystem();
#ifndef tui
	SDL_Quit();
#endif
}

void SetStartFunction(void (*startFunc)(void)) {
	_startFunc = startFunc;
}

void SetHandleEventFunction(int (*eventFunc)(void *)) {
	_handleEventFunc = eventFunc;
}

void SetUpdateFunction(void (*updateFunc)(void)) {
	_updateFunc = updateFunc;
}

void SetDrawFunction(void (*drawFunc)(void)) {
	_drawFunc = drawFunc;
}
void SetInputFunction(void (*updateFunc)(void)) {
	_inputFunc = updateFunc;
}

void Run(void) {
	bool started = Start();
	if (!started) {
		sgLogCritical("Could not start program, exiting");
	}
	if (_startFunc) _startFunc();
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(Update, 0, 1);
#else
	while (!quit) {
		Update();
	}
#endif
	Quit();
}
