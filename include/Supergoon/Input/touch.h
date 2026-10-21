#pragma once
#ifdef __cplusplus
extern "C" {
#endif
// The first finger down acts like the mouse, other fingers only count toward gestures
// Gets the touch position in game coords, -1 if not touching (still valid on the release tick)
void GetGameTouchPos(float* x, float* y);
// Checks to see if the touch overlapped here, in game coords.
int IsTouchOverlapRect(float x, float y, float width, float height);
int IsTouchDown(void);
int IsTouchJustPressed(void);
int IsTouchJustReleased(void);
// True for one tick after a single finger is lifted
int IsTouchTapped(void);
// True for one tick after a quick two finger tap that didn't move
int IsTouchTwoFingerTapped(void);
// Functions in touch.c
void UpdateTouchSystem(void);
#ifdef __cplusplus
}
#endif
