#include <Supergoon/log.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MAX_KEYS 512  // enough for ASCII + function keys

static uint8_t _currentKeyboardState[MAX_KEYS];
static uint8_t _previousKeyboardState[MAX_KEYS];

WINDOW* inputWindow = NULL;

void InitializeKeyboardImpl(void) {
	memset(_previousKeyboardState, 0, sizeof(_previousKeyboardState));
	memset(_currentKeyboardState, 0, sizeof(_currentKeyboardState));
}

// Call this once per frame
void UpdateKeyboardSystemImpl(void) {
	memcpy(_previousKeyboardState, _currentKeyboardState, sizeof(_currentKeyboardState));
	// Clear current state
	memset(_currentKeyboardState, 0, sizeof(_currentKeyboardState));

	int ch;
	// Read all key presses available this frame
	while ((ch = getch()) != ERR) {
		if (ch >= 0 && ch < MAX_KEYS) {
			_currentKeyboardState[ch] = 1;
		}
		// Handle special keys (arrows, function keys)
		// switch (ch) {
		// 	case KEY_UP:
		// 		_currentKeyboardState[256 + KEY_UP] = 1;
		// 		break;
		// 	case KEY_DOWN:
		// 		_currentKeyboardState[256 + KEY_DOWN] = 1;
		// 		break;
		// 	case KEY_LEFT:
		// 		_currentKeyboardState[256 + KEY_LEFT] = 1;
		// 		break;
		// 	case KEY_RIGHT:
		// 		_currentKeyboardState[256 + KEY_RIGHT] = 1;
		// 		break;
		// 		// Add more special keys if needed
		// }
	}
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
