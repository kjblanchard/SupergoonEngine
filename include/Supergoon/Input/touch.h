#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef union SDL_Event Event;
void handleTouchEvent(const union SDL_Event* event);
void GetGameTouchPos(float* x, float* y);
void updateTouchSystem(void);
int IsTouchOverlapRect(int x, int y, int width, int height);
int IsTouchJustPressed(void);
int IsTouchJustReleased(void);
int IsTouchHeldDown(void);
#ifdef __cplusplus
}
#endif
