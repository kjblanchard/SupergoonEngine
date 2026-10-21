#include <SDL3/SDL.h>
#include <Supergoon/Input/mouse.h>
#include <Supergoon/window.h>
#include <Supergoon/Platform/sdl/sdlWindow.h>
#include <math.h>

extern int _logicalX;
extern int _logicalY;

static bool _lastFrameMouseButtons[3] = {false};
static bool _thisFrameMouseButtons[3] = {false};
// Last mouse event was synthesized by SDL from a touch, touch.c handles those
static bool _mouseIsTouch = false;

void handleMouseEvent(const SDL_Event* event) {
	if (event->type == SDL_EVENT_MOUSE_MOTION) {
		_mouseIsTouch = event->motion.which == SDL_TOUCH_MOUSEID;
		return;
	}
	if (!(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP)) {
		return;
	}
	_mouseIsTouch = event->button.which == SDL_TOUCH_MOUSEID;
	if (_mouseIsTouch) {
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

#ifdef imgui
static float _gameViewX = 0;
static float _gameViewY = 0;
static float _gameViewW = 0;
static float _gameViewH = 0;
static int _gameViewHovered = 0;

void MouseSetGameViewRect(float x, float y, float width, float height, int hovered) {
	_gameViewX = x;
	_gameViewY = y;
	_gameViewW = width;
	_gameViewH = height;
	_gameViewHovered = hovered;
}
#endif

int WindowPointToGamePos(float mouseX, float mouseY, float* x, float* y) {
#ifdef imgui
	float viewRelX = mouseX - _gameViewX;
	float viewRelY = mouseY - _gameViewY;
	if (!_gameViewHovered || _gameViewW <= 0 || _gameViewH <= 0 ||
		viewRelX < 0 || viewRelY < 0 || viewRelX >= _gameViewW || viewRelY >= _gameViewH) {
		*x = -1;
		*y = -1;
		return 0;
	}
	*x = viewRelX / _gameViewW * _logicalX;
	*y = viewRelY / _gameViewH * _logicalY;
	return 1;
#else
	// Window size is in pixels, so convert the mouse to match on high-DPI displays
	float density = SDL_GetWindowPixelDensity(WindowGet()->Handle);
	if (density > 0) {
		mouseX *= density;
		mouseY *= density;
	}
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
		return 0;
	}
	*x = relX / scale;
	*y = relY / scale;
	return 1;
#endif
}

void GetGameMousePos(float* x, float* y) {
	// A lifted finger leaves the mouse where it was, don't report it as hovering
	if (_mouseIsTouch) {
		*x = -1;
		*y = -1;
		return;
	}
	// Window points, same units imgui uses
	float mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	WindowPointToGamePos(mouseX, mouseY, x, y);
}

int IsMouseOverlapRect(float x, float y, float width, float height) {
	float mouseX, mouseY;
	GetGameMousePos(&mouseX, &mouseY);
	if (mouseX < 0 || mouseY < 0) return 0;
	return (mouseX >= x && mouseX < x + width &&
			mouseY >= y && mouseY < y + height);
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
