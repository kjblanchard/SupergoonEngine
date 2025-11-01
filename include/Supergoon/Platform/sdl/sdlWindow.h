#pragma once
#include <SDL3/SDL_video.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Window {
	SDL_Window* Handle;
} Window;

void SetWindowOptionsImpl(int width, int height, const char* name);
void CreateWindowImpl(void);
int WindowHeightImpl(void);
int WindowWidthImpl(void);
void CloseWindowImpl(void);
Window* WindowGetImpl(void);
#ifdef __cplusplus
}
#endif
