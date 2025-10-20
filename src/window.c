#include <SDL3/SDL.h>
#include <Supergoon/graphics.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
#include <SupergoonEngine/window.h>
#ifdef tui
#include <SupergoonEngine/Platform/cursesWindow.h>
#else
#include <SupergoonEngine/Platform/sdl/sdlWindow.h>
#endif
typedef union SDL_Event Event;
#ifdef imgui
#include <Supergoon/Debug/ImGui.hpp>
#endif

void SetWindowOptions(int width, int height, const char* name) {
	SetWindowOptionsImpl(width, height, name);
}

void SetScalingOptions(int worldWidth, int worldHeight) {
	SetScalingOptionsImpl(worldWidth, worldHeight);
}

void CreateWindow(void) {
	CreateWindowImpl();
}

void DrawStart(void) {
	DrawStartImpl();
}
void DrawEnd(void) {
	DrawEndImpl();
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
