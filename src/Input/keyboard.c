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

void geInitializeKeyboard(void) {
	InitializeKeyboardStateArrays();
}

int geKeyHeldDown(int key) {
	return _previousKeyboardState[key] && _currentKeyboardState[key];
}

int geKeyJustPressed(const int key) {
	return _currentKeyboardState[key] && !_previousKeyboardState[key];
}

int geKeyJustReleased(const int key) {
	return !_currentKeyboardState[key] && _previousKeyboardState[key];
}

void geUpdateKeyboard(void) {
	memcpy(_previousKeyboardState, _currentKeyboardState, sizeof(Uint8) * SDL_SCANCODE_COUNT);
	memcpy(_currentKeyboardState, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_SCANCODE_COUNT);
}
