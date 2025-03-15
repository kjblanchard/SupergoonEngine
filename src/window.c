#include <SDL3/SDL.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
#ifdef imgui
#include <Supergoon/Debug/ImGui.hpp>

#endif

Window* _window = NULL;
Renderer* _renderer = NULL;
static int _windowWidth = 0;
static int _windowHeight = 0;
static const char* _windowName = NULL;

void CreateWindow(void) {
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	int width = (_windowWidth != 0) ? _windowWidth : 640;
	int height = (_windowHeight != 0) ? _windowHeight : 480;
	const char* name = (_windowName != NULL) ? _windowName : "Game";

#ifdef imgui
	// Make the window start bigger if we are debugging.
	width *= 2;
	height *= 2;
#endif
	int flags = SDL_WINDOW_RESIZABLE;
	if (!SDL_CreateWindowAndRenderer(name, width, height, flags, &_window, &_renderer)) {
		sgLogCritical("Could not load window, error, %s", SDL_GetError());
	}
#ifdef imgui
	InitializeImGui();
#endif
}

Window* GetGameWindow(void) {
	return _window;
}
Renderer* GetGameRenderer(void) {
	return _renderer;
}
void DrawStart(void) {
	SDL_RenderClear(_renderer);
#ifdef imgui
	StartImGuiFrame();
	DrawImGui();
#endif
}
void DrawEnd(void) {
	SDL_RenderPresent(_renderer);
}
void SetWindowOptions(int width, int height, const char* name) {
	_windowWidth = width;
	_windowHeight = height;
	_windowName = name;
}
