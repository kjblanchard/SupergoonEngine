#include <SDL3/SDL.h>
#include <Supergoon/Input/mouse.h>
#include <Supergoon/Input/touch.h>
#include <Supergoon/Platform/sdl/sdlWindow.h>
#include <Supergoon/window.h>
#include <math.h>

// Longest a two finger tap can be held, and how far the primary finger can move (normalized window units) before it's a drag
#define TWO_FINGER_TAP_MAX_NS (300 * SDL_NS_PER_MS)
#define TAP_MAX_MOVE 0.02f

static bool _lastFrameTouch = false;
static bool _thisFrameTouch = false;
// First finger down, the one that acts like the mouse
static SDL_FingerID _primaryFinger = 0;
static bool _hasPrimaryFinger = false;
// Last primary finger position in window points, kept after release so release ticks can check overlap
static float _touchWinX = -1.0f;
static float _touchWinY = -1.0f;
static bool _touchPosValid = false;
// Current gesture, from first finger down to last finger up
static int _fingersDown = 0;
static int _gestureMaxFingers = 0;
static Uint64 _gestureStartNS = 0;
static bool _gestureMoved = false;
static float _gestureStartX = 0;
static float _gestureStartY = 0;
// Latched until the next tick ends, so a tap shorter than a frame isn't lost
static bool _tapped = false;
static bool _twoFingerTapped = false;

static void setTouchPos(const SDL_TouchFingerEvent* finger) {
	int w, h;
	SDL_GetWindowSize(WindowGet()->Handle, &w, &h);
	_touchWinX = finger->x * w;
	_touchWinY = finger->y * h;
	_touchPosValid = true;
}

static void onFingerDown(const SDL_TouchFingerEvent* finger) {
	if (_fingersDown == 0) {
		_gestureMaxFingers = 0;
		_gestureStartNS = SDL_GetTicksNS();
		_gestureMoved = false;
		_gestureStartX = finger->x;
		_gestureStartY = finger->y;
	}
	++_fingersDown;
	if (_fingersDown > _gestureMaxFingers) _gestureMaxFingers = _fingersDown;
	if (!_hasPrimaryFinger) {
		_hasPrimaryFinger = true;
		_primaryFinger = finger->fingerID;
		_thisFrameTouch = true;
		setTouchPos(finger);
	}
}

static void onFingerMotion(const SDL_TouchFingerEvent* finger) {
	// Only the primary finger is checked for drags, it moves along with any two finger drag
	if (!_hasPrimaryFinger || finger->fingerID != _primaryFinger) return;
	setTouchPos(finger);
	if (fabsf(finger->x - _gestureStartX) > TAP_MAX_MOVE || fabsf(finger->y - _gestureStartY) > TAP_MAX_MOVE) {
		_gestureMoved = true;
	}
}

static void onFingerUp(const SDL_TouchFingerEvent* finger) {
	if (_fingersDown > 0) --_fingersDown;
	if (_hasPrimaryFinger && finger->fingerID == _primaryFinger) {
		_hasPrimaryFinger = false;
		_thisFrameTouch = false;
		setTouchPos(finger);
	}
	if (_fingersDown > 0) return;
	if (_gestureMaxFingers == 1) {
		_tapped = true;
	} else if (_gestureMaxFingers == 2 && !_gestureMoved &&
			   SDL_GetTicksNS() - _gestureStartNS <= TWO_FINGER_TAP_MAX_NS) {
		_twoFingerTapped = true;
	}
}

void handleTouchEvent(const SDL_Event* event) {
	if (!(event->type == SDL_EVENT_FINGER_DOWN ||
		  event->type == SDL_EVENT_FINGER_UP ||
		  event->type == SDL_EVENT_FINGER_MOTION ||
		  event->type == SDL_EVENT_FINGER_CANCELED)) {
		return;
	}
	const SDL_TouchFingerEvent* finger = &event->tfinger;
	// Only touchscreens, not trackpads or touches SDL makes from the mouse
	if (finger->touchID == SDL_MOUSE_TOUCHID || SDL_GetTouchDeviceType(finger->touchID) != SDL_TOUCH_DEVICE_DIRECT) {
		return;
	}
	switch (event->type) {
		case SDL_EVENT_FINGER_DOWN:
			onFingerDown(finger);
			break;
		case SDL_EVENT_FINGER_MOTION:
			onFingerMotion(finger);
			break;
		case SDL_EVENT_FINGER_UP:
			onFingerUp(finger);
			break;
		case SDL_EVENT_FINGER_CANCELED:
			// Taken by the OS, end everything without firing taps
			_fingersDown = 0;
			_gestureMaxFingers = 0;
			_hasPrimaryFinger = false;
			_thisFrameTouch = false;
			break;
	}
}

void UpdateTouchSystem(void) {
	_lastFrameTouch = _thisFrameTouch;
	_tapped = false;
	_twoFingerTapped = false;
	// Position only lives through the release tick
	if (!_thisFrameTouch) {
		_touchPosValid = false;
	}
}

void GetGameTouchPos(float* x, float* y) {
	if (!_touchPosValid) {
		*x = -1;
		*y = -1;
		return;
	}
	WindowPointToGamePos(_touchWinX, _touchWinY, x, y);
}

int IsTouchOverlapRect(float x, float y, float width, float height) {
	float touchX, touchY;
	GetGameTouchPos(&touchX, &touchY);
	if (touchX < 0 || touchY < 0) return 0;
	return (touchX >= x && touchX < x + width &&
			touchY >= y && touchY < y + height);
}

int IsTouchJustPressed(void) {
	return _thisFrameTouch && !_lastFrameTouch;
}

int IsTouchJustReleased(void) {
	return !_thisFrameTouch && _lastFrameTouch;
}

int IsTouchDown(void) {
	return _thisFrameTouch;
}

int IsTouchTapped(void) {
	return _tapped;
}

int IsTouchTwoFingerTapped(void) {
	return _twoFingerTapped;
}
