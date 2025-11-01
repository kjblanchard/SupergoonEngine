#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Platform/sdl/sdlWindow.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
typedef union SDL_Event Event;

// Window/renderer is used as extern in graphics
Window _window;
// Renderer* _renderer = NULL;
static int _windowWidth = 0;
static int _windowHeight = 0;
int _refreshRate = 0;
int _vsyncEnabled = true;
#ifdef __EMSCRIPTEN
_vsyncEnabled = false;
#endif
// Target FPS 999 means there will be no delay in the engine, so probably use vsync
int TARGET_FPS = 60;

Texture* _fullScreenTexture;
static char* _windowName = NULL;
// static Texture* testTexture = NULL;
static void onWindowResize(void);

void windowEventHandler(Event* event) {
	if (event->type == SDL_EVENT_WINDOW_RESIZED) {
		SDL_GetWindowSizeInPixels(_window.Handle, &_windowWidth, &_windowHeight);
		onWindowResize();
	}
}

static void onWindowResize(void) {
	sgLogDebug("Setting window size %d, %d", _windowWidth, _windowHeight);
	if (!SDL_SetWindowSize(_window.Handle, _windowWidth, _windowHeight)) {
		sgLogError("Could not set window size, %s", SDL_GetError());
	}
	if (!SDL_SetWindowTitle(_window.Handle, _windowName)) {
		sgLogError("Could not set window title");
	}
	GraphicsWindowResizeEvent(_windowWidth, _windowHeight);
}

void SetWindowOptionsImpl(int width, int height, const char* name) {
	_windowWidth = width;
	_windowHeight = height;
	if (_windowName) {
		SDL_free(_windowName);
		_windowName = NULL;
	}
	_windowName = strdup(name);
	if (_window.Handle) {
		onWindowResize();
	}
}

void CreateWindowImpl(void) {
	_windowWidth = (_windowWidth != 0) ? _windowWidth : 640;
	_windowHeight = (_windowHeight != 0) ? _windowHeight : 480;
	const char* name = (_windowName != NULL) ? _windowName : "Game";

	int flags = SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE;
#ifdef sdlglbackend
	flags |= SDL_WINDOW_OPENGL;
#endif
	_window.Handle = SDL_CreateWindow(name, _windowWidth, _windowHeight, flags);
	if (!_window.Handle) {
		sgLogCritical("Could not create window, error, %s", SDL_GetError());
	}
	SDL_RaiseWindow(_window.Handle);
	onWindowResize();
	SDL_SetWindowPosition(_window.Handle, 0, 0);
}

int WindowHeightImpl(void) {
	return _windowHeight;
}
int WindowWidthImpl(void) {
	return _windowWidth;
}
void CloseWindowImpl(void) {
	SDL_DestroyWindow(_window.Handle);
}

int getRefreshRate(void) {
	int displayIndex = SDL_GetDisplayForWindow(_window.Handle);
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

Window* WindowGetImpl(void) {
	return &_window;
}
