#pragma once
#include <SDL3/SDL_video.h>
#ifdef __cplusplus
extern "C" {
#endif

void SetWindowOptionsImpl(int width, int height, const char* name);
void CreateWindowImpl(void);
int WindowHeightImpl(void);
int WindowWidthImpl(void);
void CloseWindowImpl(void);
extern SDL_Window* _window;
#ifdef __cplusplus
}
#endif
