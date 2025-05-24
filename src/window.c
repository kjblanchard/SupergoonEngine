#include <SDL3/SDL.h>
#include <Supergoon/graphics.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
#include <SupergoonEngine/window.h>
typedef union SDL_Event Event;
#ifdef imgui
#include <Supergoon/Debug/ImGui.hpp>

#endif

// Window/renderer is used as extern in graphics
Window* _window = NULL;
Renderer* _renderer = NULL;
static int _windowWidth = 0;
static int _windowHeight = 0;
// Used in debug, so non static
int _logicalWidth = 0;
int _logicalHeight = 0;
// scale values
// top left of the game, for use fine finding mouse coords
int _gameImagePosX = 0;
// top left of the game, for use fine finding mouse coords
int _gameImagePosY = 0;
// Scale factor of the game image
float _gameImageScale = 0;
// Actual width of the game window
int _gameImageWidth = 0;
// actual height of the game window
int _gameImageHeight = 0;
int _renderTargetWidth = 0;
int _renderTargetHeight = 0;
int _refreshRate = 0;
int _vsyncEnabled = true;

Texture* _imguiGameTexture;
static const char* _windowName = NULL;
#ifdef imgui
// Defined in engine.c
extern bool _isGameSimulatorRunning;
#endif
// static Texture* testTexture = NULL;
static void onWindowResize(void);

void windowEventHandler(Event* event) {
	if (!(event->type == SDL_EVENT_WINDOW_RESIZED)) {
		return;
	}
	if (event->type == SDL_EVENT_WINDOW_RESIZED) {
		SDL_GetWindowSizeInPixels(_window, &_windowWidth, &_windowHeight);
		onWindowResize();
	}
}

void SetWindowOptions(int width, int height, const char* name) {
	_windowWidth = width;
	_windowHeight = height;
#ifdef imgui
	// Make the window start bigger if we are debugging.
	_windowWidth *= 2;
	_windowHeight *= 2;
#endif
	_windowName = name;
	if (_window) {
		onWindowResize();
	}
}

void SetScalingOptions(int worldWidth, int worldHeight) {
	_logicalWidth = worldWidth;
	_logicalHeight = worldHeight;
	if (_window) {
		onWindowResize();
	}
}

static void onWindowResize(void) {
	// TODO Cache all window information, this should be updated if the window size changes, currently it doesn't ever.
	SDL_SetWindowSize(_window, _windowWidth, _windowHeight);
	SDL_SetWindowTitle(_window, _windowName);
#ifndef imgui
	if (_logicalHeight && _logicalWidth) {
		SDL_SetRenderLogicalPresentation(_renderer, _logicalWidth, _logicalHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
	}
#endif
	int windowW, windowH;
	// TODO if for some reason the display is drawn offscreen, it will become letterboxed, not sure if problem
	SDL_GetRenderOutputSize(_renderer, &windowW, &windowH);
	int scaleX = windowW / _logicalWidth;
	int scaleY = windowH / _logicalHeight;
	int scale = (scaleX < scaleY) ? scaleX : scaleY;
	_gameImageScale = (float)scale;
	_gameImageWidth = _logicalWidth * scale;
	_gameImageHeight = _logicalHeight * scale;
	_gameImagePosX = (windowW - _gameImageWidth) / 2;
	_gameImagePosY = (windowH - _gameImageHeight) / 2;
	//  Create texture that the game will be drawn to.  ImGui will Draw to this in the Game window, otherwise the game will draw to this.
	_renderTargetWidth = _logicalWidth ? _logicalWidth : windowW;
	_renderTargetHeight = _logicalHeight ? _logicalHeight : windowH;
	if (_imguiGameTexture) {
		UnloadTexture(_imguiGameTexture);
	}
	_imguiGameTexture = CreateRenderTargetTexture(_renderTargetWidth, _renderTargetHeight, (sgColor){0, 0, 0, 255});
}

void CreateWindow(void) {
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	_windowWidth = (_windowWidth != 0) ? _windowWidth : 640;
	_windowHeight = (_windowHeight != 0) ? _windowHeight : 480;
	const char* name = (_windowName != NULL) ? _windowName : "Game";

#ifdef imgui
	int flags = SDL_WINDOW_RESIZABLE;
#else
	int flags = 0;
#endif
	if (!SDL_CreateWindowAndRenderer(name, _windowWidth, _windowHeight, flags, &_window, &_renderer)) {
		sgLogCritical("Could not load window, error, %s", SDL_GetError());
	}
	SDL_SetRenderVSync(_renderer, _vsyncEnabled);
	getRefreshRate();
#ifdef imgui
	InitializeImGui();
#endif
	onWindowResize();
	SDL_SetWindowPosition(_window, 0, 0);
}

void DrawStart(void) {
	// Clear the screen
	SDL_RenderClear(_renderer);
#ifdef imgui
	if (_isGameSimulatorRunning) {
#endif
		// Clear the texture we draw from
		SDL_SetRenderTarget(_renderer, _imguiGameTexture);
		SDL_RenderClear(_renderer);
#ifdef imgui
	}
	StartImGuiFrame();
#endif
}
void DrawEnd(void) {
	// Rectangle dst = {0, 32, 32, 32};
	// DrawTexture(testTexture, &dst, &dst);
	SDL_SetRenderTarget(_renderer, NULL);
#ifndef imgui
	SDL_FRect dest = {0, 0, _renderTargetWidth, _renderTargetHeight};
	SDL_RenderTexture(_renderer, _imguiGameTexture, &dest, &dest);
#else
	DrawImGui();
#endif
	SDL_RenderPresent(_renderer);
}

void DrawTexture(Texture* texture, RectangleF* dst, RectangleF* src) {
	SDL_RenderTexture(_renderer, texture, src, dst);
}

int WindowHeight(void) {
	return _windowHeight;
}
int WindowWidth(void) {
	return _windowWidth;
}
void CloseWindow(void) {
	UnloadTexture(_imguiGameTexture);
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
}

int getRefreshRate(void) {
	int displayIndex = SDL_GetDisplayForWindow(_window);
	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(displayIndex);
	if (!mode) {
		sgLogWarn("Failed to get display mode, %s", SDL_GetError());
		_refreshRate = 60;
		return _refreshRate;
	}
	_refreshRate = mode->refresh_rate;
	if (_refreshRate == 0) _refreshRate = 60;
	return _refreshRate;
}
