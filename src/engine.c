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
#include <Supergoon/Input/touch.h>
#include <Supergoon/Platform/sdl/sdl.h>
#include <Supergoon/camera.h>
#include <Supergoon/engine.h>
#include <Supergoon/events.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/map.h>
#include <Supergoon/services.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <Supergoon/text.h>
#include <Supergoon/window.h>
#include <sgtools/log.h>

static Uint64 _previousNS = 0;
static Uint64 _accumulatorNS = 0;
int IsGameLoading = false;
float RenderAlpha = 0.0f;
void (*_initializeFunc)(void) = NULL;
void (*_startFunc)(void) = NULL;
void (*_updateFunc)(void) = NULL;
void (*_drawFunc)(void) = NULL;
void (*_quitFunc)(void) = NULL;
void (*_inputFunc)(void) = NULL;
int (*_handleEventFunc)(void*) = NULL;
void (*_graphicsPostFBODrawUIFunc)(void) = NULL;
#define FIXED_TIMESTEP_NS 16666666ULL  // 60 FPS
									   //
static void initializeEngineInternal(void) {
	InitializeSdl();
	sgInitializeLogSystem("errors.log");
	InitializeKeyboardSystem();
	InitializeJoystickSystem();
	InitializeEventSystem();
	InitializeServiceSystem();
}

static void start(void) {
	CreateWindow();
	InitializeGraphicsSystem();
	InitializeTextSystem();
	InitializeAudioSystem();
	_previousNS = SDL_GetTicksNS();
}

/* static void handleFramerate(Uint64* now) { */
/* 	#ifdef __EMSCRIPTEN__ */
/* 		return; */
/* 	#endif */
/* 		int refreshRate = GraphicsGetTargetRefreshRate(); */
/* 		if (refreshRate != 999) {  // If we are doing a capped frame rate, we should also wait between frames. */
/* 			uint64_t current = getCurrentMSTicks(); */
/* 			Uint64 elapsedMS = current - _previousNS; */
/* 			const Uint64 FRAME_DURATION_MS = 1000 / refreshRate; */
/* 			if (elapsedMS < FRAME_DURATION_MS) { */
/* 				sgSleepMS(FRAME_DURATION_MS - elapsedMS); */
/* 			} */
/* 		} */
/* } */

static void draw(void) {
	DrawStart();
	DrawCurrentMap();
	DrawSpriteSystem();
	if (_drawFunc) _drawFunc();
	DrawUIStart();
	if (_graphicsPostFBODrawUIFunc) _graphicsPostFBODrawUIFunc();
	DrawEnd();
}

static void update(void) {
	const int MAX_TICKS_PER_FRAME = 5;
	Uint64 now = SDL_GetTicksNS();
	Uint64 frameTime = now - _previousNS;
	_previousNS = now;
	_accumulatorNS += frameTime;
	DeltaTimeSeconds = (float)FIXED_TIMESTEP_NS / (float)SDL_NS_PER_SECOND;
	DeltaTimeMilliseconds = (float)FIXED_TIMESTEP_NS / 1000000.0f;
	int ticks = 0;
	int maxTicksThisFrame = MAX_TICKS_PER_FRAME;
	while (_accumulatorNS >= FIXED_TIMESTEP_NS && ticks < maxTicksThisFrame) {
		SnapshotSpritePositions();
		UpdateAudioSystem();
		UpdateKeyboardSystem();
		UpdateCurrentMap();
		if (_inputFunc) _inputFunc();
		UpdateAnimators();
		if (_updateFunc) _updateFunc();
		UpdateCameraSystem();
		UpdateControllerSystem();
		UpdateMouseSystem();
		UpdateTouchSystem();
		UpdateServiceSystem();
		_accumulatorNS -= FIXED_TIMESTEP_NS;
		++ticks;
		now = SDL_GetTicksNS();
	}
	// Spiral of death
	if (ticks == MAX_TICKS_PER_FRAME && _accumulatorNS >= FIXED_TIMESTEP_NS) {
		_accumulatorNS = 0;
		sgLogDebug("Warning: too many ticks this frame, capping updates to avoid spiral of death");
	}
	RenderAlpha = (float)_accumulatorNS / (float)FIXED_TIMESTEP_NS;
	draw();
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
	sgShutdownLogSystem();
	ShutdownServiceSystem();
}

void SetStartFunction(void (*startFunc)(void)) { _startFunc = startFunc; }
void SetHandleEventFunction(int (*eventFunc)(void*)) { _handleEventFunc = eventFunc; }
void SetInitializeFunction(void (*initializeFunc)(void)) { _initializeFunc = initializeFunc; }
void SetUpdateFunction(void (*updateFunc)(void)) { _updateFunc = updateFunc; }
void SetDrawFunction(void (*drawFunc)(void)) { _drawFunc = drawFunc; }
void SetDrawUIFunction(void (*drawUIFunc)(void)) { _graphicsPostFBODrawUIFunc = drawUIFunc; }
void SetInputFunction(void (*updateFunc)(void)) { _inputFunc = updateFunc; }
void SetQuitFunction(void (*quitFunc)(void)) { _quitFunc = quitFunc; }

SDL_AppResult SDL_AppInit(void** appState, int argc, char* argv[]) {
	initializeEngineInternal();
	InitializeEngineFunctions();
	if (_initializeFunc) _initializeFunc();
	start();
	if (_startFunc) _startFunc();
	return SDL_APP_CONTINUE;
}

// Event handlers return if the game should quit
SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event) {
	if (HandleEvents(event)) return SDL_APP_SUCCESS;
	geHandleJoystickEvent(event);
	if (_handleEventFunc && _handleEventFunc(event)) return SDL_APP_SUCCESS;
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appState) {
	update();
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appState, SDL_AppResult result) {
    sgLogError("Quitting");
	Quit();
}
