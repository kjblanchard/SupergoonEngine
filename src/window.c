#include <SDL3/SDL.h>
#include <Supergoon/graphics.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
#ifdef imgui
#include <Supergoon/Debug/ImGui.hpp>

#endif

// Window/renderer is used as extern in graphics
Window* _window = NULL;
Renderer* _renderer = NULL;
static Texture* _imguiGameTexture;
static int _windowWidth = 0;
static int _windowHeight = 0;
static int _logicalWidth = 0;
static int _logicalHeight = 0;
static const char* _windowName = NULL;
static Texture* testTexture = NULL;

void SetWindowOptions(int width, int height, const char* name) {
	_windowWidth = width;
	_windowHeight = height;
	_windowName = name;
}

void SetScalingOptions(int worldWidth, int worldHeight) {
	_logicalWidth = worldWidth;
	_logicalHeight = worldHeight;
}

void CreateWindow(void) {
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	_windowWidth = (_windowWidth != 0) ? _windowWidth : 640;
	_windowHeight = (_windowHeight != 0) ? _windowHeight : 480;
	const char* name = (_windowName != NULL) ? _windowName : "Game";

#ifdef imgui
	// Make the window start bigger if we are debugging.
	_windowWidth *= 2;
	_windowHeight *= 2;
#endif
	int flags = SDL_WINDOW_RESIZABLE;
	if (!SDL_CreateWindowAndRenderer(name, _windowWidth, _windowHeight, flags, &_window, &_renderer)) {
		sgLogCritical("Could not load window, error, %s", SDL_GetError());
	}
#ifdef imgui
	InitializeImGui();
#else
	if (_logicalHeight && _logicalWidth) {
		SDL_SetRenderLogicalPresentation(_renderer, _logicalWidth, _logicalHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
	}
#endif
	//  Create texture that the game will be drawn to.  ImGui will Draw to this in the Game window, otherwise the game will draw to this.
	int renderTargetWidth = _logicalWidth ? _logicalWidth : _windowWidth;
	int renderTargetHeight = _logicalHeight ? _logicalHeight : _windowHeight;
	_imguiGameTexture = CreateRenderTargetTexture(renderTargetWidth, renderTargetHeight, (sgColor){0, 0, 0, 255});
	testTexture = CreateTextureFromIndexedBMP("player1");
}

void DrawStart(void) {
	// Clear the screen
	SDL_RenderClear(_renderer);
	// Clear the texture we draw from
	SDL_SetRenderTarget(_renderer, _imguiGameTexture);
	SDL_RenderClear(_renderer);
	Rectangle dst = {0, 0, 32, 32};
	DrawTexture(testTexture, &dst, &dst);
#ifdef imgui
	StartImGuiFrame();
#endif
}
void DrawEnd(void) {
	SDL_SetRenderTarget(_renderer, NULL);
	SDL_FRect dest = {0, 0, _windowWidth, _windowHeight};
	SDL_RenderTexture(_renderer, _imguiGameTexture, NULL, &dest);
#ifdef imgui
	DrawImGui();
#endif
	SDL_RenderPresent(_renderer);
}

void DrawTexture(Texture* texture, Rectangle* dst, Rectangle* src) {
	SDL_RenderTexture(_renderer, texture, src, dst);
}
