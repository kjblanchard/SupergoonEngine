#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// Gets the mouse position in the game window
// when imgui, it needs to translate to the game, otherwise just returns mouse pos in window.
void GetGameMousePos(float* x, float* y);

int IsMouseOverlap(int x, int y, int width, int height);

#ifdef __cplusplus
}
#endif
