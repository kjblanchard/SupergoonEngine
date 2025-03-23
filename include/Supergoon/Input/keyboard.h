/**
 * @file keyboard.h
 * @author Kevin Blanchard (kevin@supergoon.com)
 * @brief Handles keyboard events
 * @version 0.1
 * @date 2023-10-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef union SDL_Event SDL_Event;

void InitializeKeyboardSystem(void);
void UpdateKeyboardSystem(void);
int IsKeyboardKeyJustReleased(const int key);
int IsKeyboardKeyJustPressed(const int key);
int IsKeyboardKeyHeldDown(int key);

#ifdef __cplusplus
}
#endif
