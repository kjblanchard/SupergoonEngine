#include <Supergoon/Input/keyboard.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/Platform/cursesInput.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static uint8_t _currentKeyboardState[MAX_KEYS];
static uint8_t _previousKeyboardState[MAX_KEYS];
char _keysPressedThisFrame[MAX_KEYS];

WINDOW* inputWindow = NULL;

void InitializeKeyboardImpl(void) {
	memset(_previousKeyboardState, 0, sizeof(_previousKeyboardState));
	memset(_currentKeyboardState, 0, sizeof(_currentKeyboardState));
}

void UpdateKeyboardSystemImpl(void) {
	memcpy(_previousKeyboardState, _currentKeyboardState, sizeof(_currentKeyboardState));
	memset(_currentKeyboardState, 0, sizeof(_currentKeyboardState));

	int ch;
	int numKeysPressed = 0;
	while ((ch = getch()) != ERR) {
		if (ch >= 0 && ch < MAX_KEYS) {
			_currentKeyboardState[ch] = 1;
			_keysPressedThisFrame[numKeysPressed] = ch;
			++numKeysPressed;
		}
	}
	_keysPressedThisFrame[numKeysPressed] = '\0';
}

int IsKeyboardKeyJustReleasedImpl(const int key) {
	return !_currentKeyboardState[key] && _previousKeyboardState[key];
}

int IsKeyboardKeyJustPressedImpl(const int key) {
	return _currentKeyboardState[key] && !_previousKeyboardState[key];
}

int IsKeyboardKeyHeldDownImpl(const int key) {
	return _currentKeyboardState[key] && _previousKeyboardState[key];
}

int IsKeyboardKeyDownImpl(const int key) {
	return _currentKeyboardState[key];
}
