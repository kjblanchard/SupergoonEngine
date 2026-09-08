#include <SDL3/SDL.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/window.h>

// From openglgraphics
extern RectangleF _worldRect;
extern int _scaleX;

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
	float offsetX = _worldRect.x;
	float offsetY = _worldRect.y;
	float relX = mouseX - offsetX;
	float relY = mouseY - offsetY;
	// If it is in the letterboxing, we should return -1
	if (relX < 0 || relY < 0 ||
		relX >= _worldRect.w || relY >= _worldRect.h) {
		*x = -1;
		*y = -1;
		return;
	}
	// Handle scale factor
	*x = relX / _scaleX;
	*y = relY / _scaleX;
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
