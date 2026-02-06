#include <SDL3/SDL_init.h>
#include <ogg/ogg.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <Supergoon/Animation/animator.h>
#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Input/joystick.h>
#include <Supergoon/Input/keyboard.h>
#include <Supergoon/Input/mouse.h>
#include <Supergoon/Platform/sdl/sdl.h>
#include <Supergoon/camera.h>
#include <Supergoon/engine.h>
#include <Supergoon/events.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/map.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <Supergoon/text.h>
#include <Supergoon/tools.h>
#include <Supergoon/window.h>

static Uint64 _previousNS = 0;
static Uint64 _accumulatorNS = 0;
/* static float _deltaTimeSeconds = 0; */
#define FIXED_TIMESTEP_NS 16666666ULL  // 60 FPS

static void start(void) {
	InitializeSdl();
	InitializeLogSystem();
	InitializeKeyboardSystem();
	InitializeJoystickSystem();
	InitializeEventSystem();
	CreateWindow();
	InitializeGraphicsSystem();
	InitializeTextSystem();
	InitializeAudioSystem();
	_previousNS = getCurrentNSTicks();
}

static void handleFramerate(Uint64* now) {
	/* #ifdef __EMSCRIPTEN__ */
	/* 	return; */
	/* #endif */
	/* 	int refreshRate = GraphicsGetTargetRefreshRate(); */
	/* 	if (refreshRate != 999) {  // If we are doing a capped frame rate, we should also wait between frames. */
	/* 		uint64_t current = getCurrentMSTicks(); */
	/* 		Uint64 elapsedMS = current - _previousNS; */
	/* 		const Uint64 FRAME_DURATION_MS = 1000 / refreshRate; */
	/* 		if (elapsedMS < FRAME_DURATION_MS) { */
	/* 			sgSleepMS(FRAME_DURATION_MS - elapsedMS); */
	/* 		} */
	/* 	} */
}

static void draw(void) {
	DrawStart();
	DrawCurrentMap();
	DrawSpriteSystem();
	if (_drawFunc) _drawFunc();
	DrawEnd();
}
static void update(void) {
	Uint64 now = getCurrentNSTicks();
	_accumulatorNS += now - _previousNS;
	/* sgLogDebug("Adding accumulator %d", now - _previousNS); */
	int ticks = 0;
	while (_accumulatorNS >= FIXED_TIMESTEP_NS) {
		DeltaTimeSeconds = (float)FIXED_TIMESTEP_NS / (float)SDL_NS_PER_SECOND;
		DeltaTimeMilliseconds = (float)FIXED_TIMESTEP_NS / 1000000.0f;
		UpdateCameraSystem();
		UpdateAudioSystem();
		UpdateKeyboardSystem();
		UpdateCurrentMap();
		if (_inputFunc) _inputFunc();
		UpdateAnimators();
		if (_updateFunc) _updateFunc();
		UpdateControllerSystem();
		UpdateMouseSystem();
		_accumulatorNS -= FIXED_TIMESTEP_NS;
		if(ticks++ > 2) break;
	}
	draw();
	_previousNS = now;
	/* handleFramerate(&now); */
}

static void Quit(void) {
	if (_quitFunc) _quitFunc();
	ShutdownMapSystem();
	ShutdownSpriteSystem();
	ShutdownJoystickSystem();
	ShutdownGraphicsSystem();
	ShutdownAudioSystem();
	CloseWindow();
	ShutdownEngineSilesystem();
	ShutdownLogSystem();
}

void SetHandleEventFunction(int (*eventFunc)(void*)) { _handleEventFunc = eventFunc; }
void SetUpdateFunction(void (*updateFunc)(void)) { _updateFunc = updateFunc; }
void SetDrawFunction(void (*drawFunc)(void)) { _drawFunc = drawFunc; }
void SetInputFunction(void (*updateFunc)(void)) { _inputFunc = updateFunc; }
void SetQuitFunction(void (*quitFunc)(void)) { _quitFunc = quitFunc; }

SDL_AppResult SDL_AppInit(void** appState, int argc, char* argv[]) {
	if (_initializeFunc) _initializeFunc();
	start();
	if (_startFunc) _startFunc();
	return SDL_APP_CONTINUE;
}

// Event handlers return if the game should quit
SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event) {
	if (HandleEvents(event)) return SDL_APP_SUCCESS;
	if (_handleEventFunc && _handleEventFunc(event)) return SDL_APP_SUCCESS;
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appState) {
	update();
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appState, SDL_AppResult result) {
	Quit();
}
