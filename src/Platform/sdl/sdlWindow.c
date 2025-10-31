#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/camera.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
typedef union SDL_Event Event;

// Window/renderer is used as extern in graphics
Window* _window = NULL;
// Renderer* _renderer = NULL;
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
// Without vsync, there is always some kind of jitter.
int _vsyncEnabled = true;
#ifdef __EMSCRIPTEN
_vsyncEnabled = false;
#endif
// Target FPS 999 means there will be no delay in the engine, so probably use vsync
int TARGET_FPS = 999;

Texture* _fullScreenTexture;
static char* _windowName = NULL;
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

void SetWindowOptionsImpl(int width, int height, const char* name) {
	_windowWidth = width;
	_windowHeight = height;
	if (_windowName) {
		SDL_free(_windowName);
		_windowName = NULL;
	}
	_windowName = strdup(name);
	if (_window) {
		onWindowResize();
	}
}

static void onWindowResize(void) {
	sgLogDebug("Setting window size %d, %d", _windowWidth, _windowHeight);
	if (!SDL_SetWindowSize(_window, _windowWidth, _windowHeight)) {
		sgLogError("Could not set window size, %s", SDL_GetError());
	}
	if (!SDL_SetWindowTitle(_window, _windowName)) {
		sgLogError("Could not set window title");
	}
	GraphicsWindowResizeEvent(_windowWidth, _windowHeight);

	// Set the scaling
	if (_logicalHeight && _logicalWidth) {
	}
	int scaleX = 1.0;
	int scaleY = 1.0;
	// if (_logicalWidth) {
	// 	scaleX = windowW / _logicalWidth;
	// 	scaleY = windowH / _logicalHeight;
	// }
	int scale = (scaleX < scaleY) ? scaleX : scaleY;
	_gameImageScale = (float)scale;
	_gameImageWidth = _logicalWidth * scale;
	_gameImageHeight = _logicalHeight * scale;
	// _gameImagePosX = (windowW - _gameImageWidth) / 2;
	// _gameImagePosY = (windowH - _gameImageHeight) / 2;
	//  Create texture that the game will be drawn to.
	// _renderTargetWidth = _logicalWidth ? _logicalWidth : windowW;
	// _renderTargetHeight = _logicalHeight ? _logicalHeight : windowH;
	// if (_fullScreenTexture) {
	// 	UnloadTexture(_fullScreenTexture);
	// }
	// _fullScreenTexture = CreateRenderTargetTexture(_renderTargetWidth, _renderTargetHeight, (sgColor){0, 0, 0, 255});
	//
	SetCameraSize(_windowWidth, _windowHeight);
}

void CreateWindowImpl(void) {
#ifndef __EMSCRIPTEN__
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
#endif
	_windowWidth = (_windowWidth != 0) ? _windowWidth : 640;
	_windowHeight = (_windowHeight != 0) ? _windowHeight : 480;
	const char* name = (_windowName != NULL) ? _windowName : "Game";

	int flags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS;
#ifdef sdlglbackend
	flags |= SDL_WINDOW_OPENGL;
#endif
	_window = SDL_CreateWindow(name, _windowWidth, _windowHeight, flags);
	if (!_window) {
		sgLogCritical("Could not create window, error, %s", SDL_GetError());
	}
	// if (!SDL_SetRenderVSync(_renderer, _vsyncEnabled)) {
	// 	sgLogWarn("Could not set vsync, %s", SDL_GetError());
	// }

	SDL_RaiseWindow(_window);
	getRefreshRate();
	onWindowResize();
	SDL_SetWindowPosition(_window, 0, 0);
}

int WindowHeightImpl(void) {
	return _windowHeight;
}
int WindowWidthImpl(void) {
	return _windowWidth;
}
void CloseWindowImpl(void) {
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
