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
#include <Supergoon/map.h>
#include <Supergoon/services.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <Supergoon/text.h>
#include <Supergoon/window.h>
#include <Supergoon/lua.h>
#include <sgtools/log.h>

static const int timestepNS = 16666666ULL;	// 60 FPS
static const int MAX_TICKS_PER_FRAME = 5;
static Uint64 previousNS = 0;
static Uint64 accumulatorNS = 0;
static void (*initializeFunc)(void) = NULL;
static void (*startFunc)(void) = NULL;
static void (*updateFunc)(void) = NULL;
static void (*drawFunc)(void) = NULL;
static void (*quitFunc)(void) = NULL;
static void (*inputFunc)(void) = NULL;
static int (*handleEventFunc)(void*) = NULL;
static void (*graphicsPostFBODrawUIFunc)(void) = NULL;
static void initializeEngineInternal(void) {
	InitializeSdl();
	sgInitializeLogSystem("errors.log");
  InitializeLuaSystem();
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
	previousNS = SDL_GetTicksNS();
}

static void draw() {
	DrawStart();
	DrawCurrentMap();
	SpriteSystemDraw();
	if (drawFunc) drawFunc();
	DrawUIStart();
	if (graphicsPostFBODrawUIFunc) graphicsPostFBODrawUIFunc();
	DrawEnd();
}

static void update(void) {
	Uint64 now = SDL_GetTicksNS();
	Uint64 frameTime = now - previousNS;
	previousNS = now;
	accumulatorNS += frameTime;
	DeltaTimeSeconds = (float)timestepNS / (float)SDL_NS_PER_SECOND;
	DeltaTimeMilliseconds = (float)timestepNS / 1000000.0f;
	int ticks = 0;
	int maxTicksThisFrame = MAX_TICKS_PER_FRAME;
	while (accumulatorNS >= timestepNS && ticks < maxTicksThisFrame) {
		SpriteSystemUpdate();
		UpdateAudioSystem();
		UpdateKeyboardSystem();
		UpdateCurrentMap();
		if (inputFunc) inputFunc();
		UpdateAnimators();
		if (updateFunc) updateFunc();
		UpdateCameraSystem();
		UpdateControllerSystem();
		UpdateMouseSystem();
		UpdateServiceSystem();
		accumulatorNS -= timestepNS;
		++ticks;
		now = SDL_GetTicksNS();
	}
	if (ticks == MAX_TICKS_PER_FRAME && accumulatorNS >= timestepNS) {
		accumulatorNS = 0;
		sgLogDebug("Warning: too many ticks this frame, capping updates to avoid spiral of death");
	}
	float a = (float)accumulatorNS / (float)timestepNS;
	CameraSetInterpolationAlpha(a);
	draw();
}

static void Quit(void) {
	if (quitFunc) quitFunc();
	ShutdownMapSystem();
	SpriteSystemShutdown();
	ShutdownJoystickSystem();
	ShutdownGraphicsSystem();
	ShutdownAudioSystem();
	CloseWindow();
	ShutdownEngineSilesystem();
	sgShutdownLogSystem();
	ShutdownServiceSystem();
}

void SetStartFunction(void (*f)(void)) { startFunc = f; }
void SetHandleEventFunction(int (*f)(void*)) { handleEventFunc = f; }
void SetInitializeFunction(void (*f)(void)) { initializeFunc = f; }
void SetUpdateFunction(void (*f)(void)) { updateFunc = f; }
void SetDrawFunction(void (*f)(void)) { drawFunc = f; }
void SetDrawUIFunction(void (*f)(void)) { graphicsPostFBODrawUIFunc = f; }
void SetInputFunction(void (*f)(void)) { inputFunc = f; }
void SetQuitFunction(void (*f)(void)) { quitFunc = f; }

SDL_AppResult SDL_AppInit(void** appState, int argc, char* argv[]) {
	initializeEngineInternal();
	InitializeEngineFunctions();
	if (initializeFunc) initializeFunc();
	start();
	if (startFunc) startFunc();
	return SDL_APP_CONTINUE;
}

// Event handlers return if the game should quit
SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event) {
	if (HandleEvents(event)) return SDL_APP_SUCCESS;
	geHandleJoystickEvent(event);
	if (handleEventFunc && handleEventFunc(event)) return SDL_APP_SUCCESS;
	return SDL_APP_CONTINUE;
}

static int _iterateLogCount = 0;
SDL_AppResult SDL_AppIterate(void* appState) {
	if (_iterateLogCount < 3) {
		++_iterateLogCount;
	}
	update();
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appState, SDL_AppResult result) {
	sgLogError("Quitting");
	Quit();
}
