#include <Supergoon/Input/keyboard.h>
#include <Supergoon/Platform/sdl/sdlInput.h>
#include <stdbool.h>
#include <string.h>

void InitializeKeyboardSystem(void) {
	InitializeKeyboardImpl();
}

int IsKeyboardKeyHeldDown(int key) {
	return IsKeyboardKeyHeldDownImpl(key);
}

int IsKeyboardKeyJustPressed(const int key) {
	return IsKeyboardKeyJustPressedImpl(key);
}

int IsKeyboardKeyJustReleased(const int key) {
	return IsKeyboardKeyJustReleasedImpl(key);
}

int IsKeyboardKeyDown(const int key) {
	return IsKeyboardKeyDownImpl(key);
}

void UpdateKeyboardSystem(void) {
	UpdateKeyboardSystemImpl();
}
