#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void InitializeKeyboardImpl();
void UpdateKeyboardSystemImpl(void);
int IsKeyboardKeyJustReleasedImpl(const int key);
int IsKeyboardKeyJustPressedImpl(const int key);
int IsKeyboardKeyHeldDownImpl(const int key);
int IsKeyboardKeyDownImpl(const int key);

#ifdef __cplusplus
}
#endif
