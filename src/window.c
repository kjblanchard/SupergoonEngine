#include <SDL3/SDL.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Platform/sdl/sdlWindow.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
typedef union SDL_Event Event;

void SetWindowOptions(int width, int height, const char* name) {
	SetWindowOptionsImpl(width, height, name);
}

void CreateWindow(void) {
	CreateWindowImpl();
}

int WindowHeight(void) {
	return WindowHeightImpl();
}
int WindowWidth(void) {
	return WindowWidthImpl();
}
void CloseWindow(void) {
	CloseWindowImpl();
}
