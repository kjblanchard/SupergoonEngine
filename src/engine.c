
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <Supergoon/Animation/animator.h>
#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Input/joystick.h>
#include <Supergoon/Input/keyboard.h>
#include <Supergoon/Input/mouse.h>
#include <Supergoon/Lua/engine.h>
#include <Supergoon/Lua/scripting.h>
#include <Supergoon/Platform/sdl/sdl.h>
#include <Supergoon/Tweening/tween.h>
#include <Supergoon/camera.h>
#include <Supergoon/engine.h>
#include <Supergoon/events.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <Supergoon/tools.h>
#include <Supergoon/tween.h>
#include <Supergoon/ui.h>
#include <Supergoon/window.h>

static Uint64 _previousMS;
static float _deltaTimeSeconds;
// This needs to be defined in the program that will be utilizing the engine.
extern void(StartImpl)(void);
static void (*_updateFunc)(void) = NULL;
static void (*_drawFunc)(void) = NULL;
static void (*_inputFunc)(void) = NULL;
static int (*_handleEventFunc)(void *) = NULL;

static bool start(void) {
	InitializeSdl();
	InitializeLogSystem();
	InitializeKeyboardSystem();
	InitializeJoystickSystem();
	InitializeLuaSystem();
	InitializeEventSystem();
	CreateWindow();
	InitializeGraphicsSystem();
	InitializeAudioSystem();
	InitializeTweenSystem();
	// InitializeUISystem();
	RegisterAllLuaFunctions();
	_previousMS = getCurrentMSTicks();
	_deltaTimeSeconds = 0;
	return true;
}

static void handleFramerate(Uint64 *now) {
#ifdef __EMSCRIPTEN__
	return;
#endif
	if (TARGET_FPS != 999) {  // If we are doing a capped frame rate, we should also wait between frames.
		uint64_t current = getCurrentMSTicks();
		Uint64 elapsedMS = current - _previousMS;
		const Uint64 FRAME_DURATION_MS = 1000 / TARGET_FPS;
		if (elapsedMS < FRAME_DURATION_MS) {
			sgSleepMS(FRAME_DURATION_MS - elapsedMS);
		}
	}
}

static void draw(void) {
	DrawStart();
	// DrawCurrentMap();
	DrawSpriteSystem();
	if (_drawFunc) _drawFunc();
	// DrawUISystem();
	// DrawEnd();
	DrawEnd();
}

static void Update(void) {
	Uint64 now = getCurrentMSTicks();
	DeltaTimeMilliseconds = now - _previousMS;
	_previousMS = now;
	DeltaTimeSeconds = DeltaTimeMilliseconds / 1000;
	UpdateAudioSystem();
	UpdateKeyboardSystem();
	if (_inputFunc) _inputFunc();
	// UpdateUIInputSystem();
	Ticks += 1;
	// UpdateTweens();
	// UpdateAnimators();
	PushGamestateToLua();
	// GameObjectSystemUpdate();
	if (_updateFunc) _updateFunc();
	// UpdateUISystem();
	UpdateControllerSystem();
	UpdateMouseSystem();
	// UpdateTouchSystem();
	UpdateCameraSystem();
	draw();
	handleFramerate(&now);
}

static void Quit(void) {
	// ShutdownMapSystem();
	ShutdownSpriteSystem();
	ShutdownJoystickSystem();
	ShutdownGraphicsSystem();
	ShutdownLuaSystem();
	ShutdownAudioSystem();
	// ShutdownUISystem();
	CloseWindow();
	ShutdownEngineSilesystem();
	ShutdownLogSystem();
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

SDL_AppResult SDL_AppInit(void **appState, int argc, char *argv[]) {
	bool started = start();
	if (!started) {
		sgLogCritical("Could not start program, exiting");
	}
	StartImpl();
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appState, SDL_Event *event) {
	// return (SDL_AppResult)((Game *)appState)->HandleEvent(event);
	switch (event->type) {
		case SDL_EVENT_QUIT:
			sgLogWarn("Going to quit from engine");
			return SDL_APP_SUCCESS;
	}
	bool shouldQuit = false;
	if (_handleEventFunc) {
		shouldQuit = _handleEventFunc(&event);
	}
	HandleEvents(&event);
	if (shouldQuit) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appState) {
	Update();
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appState, SDL_AppResult result) {
	Quit();
}
