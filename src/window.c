#include <SDL3/SDL.h>
#include <Supergoon/Graphics/graphics.h>
#include <sgtools/log.h>
#include <Supergoon/window.h>

extern void SetWindowOptionsImpl(int width, int height, const char* name);
extern void CreateWindowImpl(void);
extern int WindowHeightImpl(void);
extern int WindowWidthImpl(void);
extern void CloseWindowImpl(void);
extern Window* WindowGetImpl(void);
extern void StartTextInputImpl(void);
extern void StopTextInputImpl(void);

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

void GeStartTextInput(void) {
	StartTextInputImpl();
}

void GeStopTextInput(void) {
	StopTextInputImpl();
}
