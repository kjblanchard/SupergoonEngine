#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#define MAX_KEYS 512  // enough for ASCII + function keys
void InitializeKeyboardImpl(void);
void UpdateKeyboardSystemImpl(void);
int IsKeyboardKeyJustReleasedImpl(const int key);
int IsKeyboardKeyJustPressedImpl(const int key);
int IsKeyboardKeyHeldDownImpl(const int key);
int IsKeyboardKeyDownImpl(const int key);
extern char _keysPressedThisFrame[MAX_KEYS];

#ifdef __cplusplus
}
#endif
