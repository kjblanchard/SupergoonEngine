#include <SDL3/SDL.h>
#include <SupergoonEngine/Platform/sdlInput.h>
#include <stdint.h>

static uint8_t _currentKeyboardState[SDL_SCANCODE_COUNT];
static uint8_t _previousKeyboardState[SDL_SCANCODE_COUNT];

// static void InitializeKeyboardStateArrays(void) {
// 	InitializeKeyboardImpl();
// }

void InitializeKeyboardImpl() {
	memset(_previousKeyboardState, 0, sizeof(Uint8) * SDL_SCANCODE_COUNT);
	memcpy(_currentKeyboardState, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_SCANCODE_COUNT);
}

void UpdateKeyboardSystemImpl(void) {
	memcpy(_previousKeyboardState, _currentKeyboardState, sizeof(Uint8) * SDL_SCANCODE_COUNT);
	memcpy(_currentKeyboardState, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_SCANCODE_COUNT);
}
int IsKeyboardKeyJustReleasedImpl(const int key) {
	return !_currentKeyboardState[key] && _previousKeyboardState[key];
}
int IsKeyboardKeyJustPressedImpl(const int key) {
	return _currentKeyboardState[key] && !_previousKeyboardState[key];
}
int IsKeyboardKeyHeldDownImpl(const int key) {
	return _previousKeyboardState[key] && _currentKeyboardState[key];
}
int IsKeyboardKeyDownImpl(const int key) {
	return _currentKeyboardState[key];
}
