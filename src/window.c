#include <SDL3/SDL.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>

extern void SetWindowOptionsImpl(int width, int height, const char* name);
extern void CreateWindowImpl(void);
extern int WindowHeightImpl(void);
extern int WindowWidthImpl(void);
extern void CloseWindowImpl(void);
extern Window* WindowGetImpl(void);

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

Window* WindowGet(void) {
	return WindowGetImpl();
}
