#include <SDL3/SDL.h>
#include <Supergoon/Input/keyboard.h>
#include <stdbool.h>
#include <string.h>

static Uint8 _currentKeyboardState[SDL_SCANCODE_COUNT];
static Uint8 _previousKeyboardState[SDL_SCANCODE_COUNT];

static void InitializeKeyboardStateArrays(void) {
	memset(_previousKeyboardState, 0, sizeof(Uint8) * SDL_SCANCODE_COUNT);
	memcpy(_currentKeyboardState, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_SCANCODE_COUNT);
}

void InitializeKeyboardSystem(void) {
	InitializeKeyboardStateArrays();
}

int IsKeyboardKeyHeldDown(int key) {
	return _previousKeyboardState[key] && _currentKeyboardState[key];
}

int IsKeyboardKeyJustPresed(const int key) {
	return _currentKeyboardState[key] && !_previousKeyboardState[key];
}

int IsKeyboardKeyJustReleased(const int key) {
	return !_currentKeyboardState[key] && _previousKeyboardState[key];
}

void UpdateKeyboardSystem(void) {
	memcpy(_previousKeyboardState, _currentKeyboardState, sizeof(Uint8) * SDL_SCANCODE_COUNT);
	memcpy(_currentKeyboardState, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_SCANCODE_COUNT);
}
