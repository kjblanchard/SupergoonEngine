#include <SDL3/SDL.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>

Window* _window = NULL;
Renderer* _renderer = NULL;
static int _windowWidth = 0;
static int _windowHeight = 0;

void CreateWindow(int width, int height, const char* name) {
	_windowWidth = width;
	_windowHeight = height;
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
#ifdef imgui
	// Make the window start bigger if we are debugging.
	width *= 2;
	height *= 2;
#endif
	int flags = SDL_WINDOW_RESIZABLE;
	if (!SDL_CreateWindowAndRenderer(name, width, height, flags, &_window, &_renderer)) {
		sgLogCritical("Could not load window, error, %s", SDL_GetError());
	}
}

Window* GetGameWindow(void) {
	return _window;
}
Renderer* GetGameRenderer(void) {
	return _renderer;
}
void DrawStart(void) {
	SDL_RenderClear(_renderer);
	// #ifdef imgui
	//   ClearRenderTargetTexture(_imguiGameTexture);
	//   // Draw everything to this target instead when building with imgui.
	//   SDL_SetRenderTarget(_renderer, _imguiGameTexture);
	//   ImGui_ImplSDLRenderer3_NewFrame();
	//   ImGui_ImplSDL3_NewFrame();
	//   ImGui::NewFrame();
	// #endif
}
void DrawEnd(void) {
	SDL_RenderPresent(_renderer);
}
