#include <SDL3/SDL.h>
#include <Supergoon/Input/touch.h>
#include <Supergoon/window.h>
#include <math.h>

extern int _logicalX;
extern int _logicalY;

static bool _lastFrameTouch = false;
static bool _thisFrameTouch = false;
static float _touchX = -1.0f;
static float _touchY = -1.0f;

void handleTouchEvent(const SDL_Event* event) {
	if (!(event->type == SDL_EVENT_FINGER_DOWN ||
		  event->type == SDL_EVENT_FINGER_UP ||
		  event->type == SDL_EVENT_FINGER_MOTION)) {
		return;
	}

	if (event->type == SDL_EVENT_FINGER_DOWN || event->type == SDL_EVENT_FINGER_MOTION) {
		_thisFrameTouch = true;

		int winW = WindowWidth();
		int winH = WindowHeight();
		float rawX = event->tfinger.x * winW;
		float rawY = event->tfinger.y * winH;

		int scaleX = winW / _logicalX;
		int scaleY = winH / _logicalY;
		int scale = (scaleX < scaleY) ? scaleX : scaleY;
		if (scale < 1) scale = 1;
		int drawW = _logicalX * scale;
		int drawH = _logicalY * scale;
		float offsetX = floorf((winW - drawW) / 2.0f);
		float offsetY = floorf((winH - drawH) / 2.0f);
		float relX = rawX - offsetX;
		float relY = rawY - offsetY;
		if (relX < 0 || relY < 0 || relX >= drawW || relY >= drawH) {
			_touchX = -1;
			_touchY = -1;
			return;
		}
		_touchX = relX / scale;
		_touchY = relY / scale;
	} else if (event->type == SDL_EVENT_FINGER_UP) {
		_thisFrameTouch = false;
		_touchX = -1.0f;
		_touchY = -1.0f;
	}
}

void UpdateTouchSystem(void) {
	_lastFrameTouch = _thisFrameTouch;
}

void GetGameTouchPos(float* x, float* y) {
	*x = _touchX;
	*y = _touchY;
}

int IsTouchOverlapRect(int x, int y, int width, int height) {
	float touchX, touchY;
	GetGameTouchPos(&touchX, &touchY);
	if (touchX < 0 || touchY < 0) return 0;

	int logicX = (int)touchX;
	int logicY = (int)touchY;

	return (logicX >= x && logicX < x + width &&
			logicY >= y && logicY < y + height);
}

int IsTouchJustPressed(void) {
	return _thisFrameTouch && !_lastFrameTouch;
}

int IsTouchJustReleased(void) {
	return !_thisFrameTouch && _lastFrameTouch;
}

int IsTouchHeldDown(void) {
	return _thisFrameTouch;
}
