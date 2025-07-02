#include <SDL3/SDL.h>
#include <Supergoon/Input/touch.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/window.h>

static bool _lastFrameTouch = false;
static bool _thisFrameTouch = false;
static float _touchX = -1.0f;
static float _touchY = -1.0f;

// Capture touch input and map it to screen-space
void handleTouchEvent(const SDL_Event* event) {
	if (!(event->type == SDL_EVENT_FINGER_DOWN ||
		  event->type == SDL_EVENT_FINGER_UP ||
		  event->type == SDL_EVENT_FINGER_MOTION)) {
		return;
	}

	if (event->type == SDL_EVENT_FINGER_DOWN || event->type == SDL_EVENT_FINGER_MOTION) {
		_thisFrameTouch = true;

		int windowW, windowH;
		if (!SDL_GetRenderOutputSize(_renderer, &windowW, &windowH)) {
			sgLogError("Failed to get render output size: %s", SDL_GetError());
			return;
		}

		// Touch positions are normalized (0.0 to 1.0), scale to window space
		_touchX = event->tfinger.x * windowW;
		_touchY = event->tfinger.y * windowH;

	} else if (event->type == SDL_EVENT_FINGER_UP) {
		_thisFrameTouch = false;
	}
}

void updateTouchSystem(void) {
	_lastFrameTouch = _thisFrameTouch;
}

void GetGameTouchPos(float* x, float* y) {
	int relX = _touchX - _gameImagePosX;
	int relY = _touchY - _gameImagePosY;

	if (_thisFrameTouch && relX >= 0 && relY >= 0 &&
		relX < _gameImageWidth && relY < _gameImageHeight) {
		*x = relX / _gameImageScale;
		*y = relY / _gameImageScale;
	} else {
		*x = -1;
		*y = -1;
	}
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
