#include <SDL3/SDL.h>
#include <Supergoon/Input/mouse.h>
#include <Supergoon/window.h>
#include <math.h>

extern int _logicalX;
extern int _logicalY;

static bool _lastFrameMouseButtons[3] = {false};
static bool _thisFrameMouseButtons[3] = {false};

void handleMouseEvent(const SDL_Event* event) {
	if (!(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP)) {
		return;
	}
	bool buttonPressed = event->type == SDL_EVENT_MOUSE_BUTTON_DOWN ? true : false;
	int buttonNum = event->button.button == SDL_BUTTON_LEFT ? 0 : event->button.button == SDL_BUTTON_RIGHT ? 1
																										   : 2;
	_thisFrameMouseButtons[buttonNum] = buttonPressed;
}

void UpdateMouseSystem(void) {
	for (int i = 0; i < 3; ++i) {
		_lastFrameMouseButtons[i] = _thisFrameMouseButtons[i];
	}
}

void GetGameMousePos(float* x, float* y) {
	float mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	int winW = WindowWidth();
	int winH = WindowHeight();
	int scaleX = winW / _logicalX;
	int scaleY = winH / _logicalY;
	int scale = (scaleX < scaleY) ? scaleX : scaleY;
	if (scale < 1) scale = 1;
	int drawW = _logicalX * scale;
	int drawH = _logicalY * scale;
	float offsetX = floorf((winW - drawW) / 2.0f);
	float offsetY = floorf((winH - drawH) / 2.0f);
	float relX = mouseX - offsetX;
	float relY = mouseY - offsetY;
	if (relX < 0 || relY < 0 || relX >= drawW || relY >= drawH) {
		*x = -1;
		*y = -1;
		return;
	}
	*x = relX / scale;
	*y = relY / scale;
}

int IsMouseOverlapRect(int x, int y, int width, int height) {
	float mouseX, mouseY;
	GetGameMousePos(&mouseX, &mouseY);
	if (mouseX < 0 || mouseY < 0) return 0;

	int logicX = (int)mouseX;
	int logicY = (int)mouseY;

	return (logicX >= x && logicX < x + width &&
			logicY >= y && logicY < y + height);
}

int IsMouseButtonJustPressed(int button) {
	return _thisFrameMouseButtons[button] && !_lastFrameMouseButtons[button];
}

int IsMouseButtonJustReleased(int button) {
	return !_thisFrameMouseButtons[button] && _lastFrameMouseButtons[button];
}

int IsMouseButtonDown(int button) {
	return _thisFrameMouseButtons[button];
}
