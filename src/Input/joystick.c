#include <SDL3/SDL.h>
#include <Supergoon/Input/joystick.h>
#include <Supergoon/log.h>
#include <math.h>
#include <string.h>

typedef struct GamePad {
	bool lastFrameButtons[SDL_GAMEPAD_BUTTON_COUNT];
	bool thisFrameButtons[SDL_GAMEPAD_BUTTON_COUNT];
	int lastFrameAxis[SDL_GAMEPAD_AXIS_COUNT];
	int thisFrameAxis[SDL_GAMEPAD_AXIS_COUNT];
	SDL_Gamepad *Gamepad;
} GamePad;

#define MAX_GAMEPADS 4
const float DEADZONE = 0.2f;  // Deadzone threshold
static int _numGamePads;
static void CountPluggedInControllers(void);
GamePad _connectedGamepads[MAX_GAMEPADS];

int geGamepadMaxPads(void) {
	return MAX_GAMEPADS;
}

static void InitializeEngineGamepad(int padId, int playerNum) {
	if (_connectedGamepads[padId].Gamepad)
		return;
	GamePad *gamepad = &_connectedGamepads[playerNum];
	SDL_Gamepad *pad = SDL_OpenGamepad(padId);
	if (!pad) {
		sgLogError("Could not open gamecontroller");
	}
	gamepad->Gamepad = pad;
	for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++) {
		gamepad->lastFrameButtons[i] = false;
		gamepad->thisFrameButtons[i] = false;
	}
	for (int i = 0; i < SDL_GAMEPAD_AXIS_COUNT; i++) {
		gamepad->lastFrameAxis[i] = 0;
		gamepad->thisFrameAxis[i] = 0;
	}
}

void geInitializeJoysticks(void) {
	SDL_SetGamepadEventsEnabled(true);
	CountPluggedInControllers();
}

void geHandleJoystickEvent(const SDL_Event *event) {
	switch (event->type) {
		case SDL_EVENT_GAMEPAD_ADDED:
			sgLogDebug("Controller added %s", event->cdevice.which);
			CountPluggedInControllers();
			break;

		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			if (event->gbutton.button == SDL_GAMEPAD_BUTTON_LEFT_SHOULDER) {
				SDL_Event quit;
				quit.type = SDL_EVENT_QUIT;
				SDL_PushEvent(&quit);
			}
			sgLogWarn("Button pressed! %d", event->gbutton.button);
			for (int i = 0; i < _numGamePads; i++) {
				if (event->gbutton.which == SDL_GetJoystickID(SDL_GetGamepadJoystick(_connectedGamepads[i].Gamepad))) {
					_connectedGamepads[i].thisFrameButtons[event->gbutton.button] = true;
				}
			}
			break;
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			for (int i = 0; i < _numGamePads; i++) {
				if (event->gbutton.which == SDL_GetJoystickID(SDL_GetGamepadJoystick(_connectedGamepads[i].Gamepad))) {
					_connectedGamepads[i].thisFrameButtons[event->gbutton.button] = false;
				}
			}
			break;
		case SDL_EVENT_GAMEPAD_AXIS_MOTION:
			for (int i = 0; i < _numGamePads; i++) {
				if (event->gbutton.which == SDL_GetJoystickID(SDL_GetGamepadJoystick(_connectedGamepads[i].Gamepad))) {
					_connectedGamepads[i].thisFrameAxis[event->gaxis.axis] = event->gaxis.value;
				}
			}
			break;
		default:
			break;
	}
}

static void CountPluggedInControllers(void) {
	int numJoysticks;
	SDL_JoystickID *joysticks = SDL_GetJoysticks(&numJoysticks);
	sgLogDebug("There is a total of %d controllers initializing", numJoysticks);
	_numGamePads = 0;
	for (int i = 0; i < numJoysticks; i++)
		if (SDL_IsGamepad(joysticks[i])) {
			_numGamePads++;
			InitializeEngineGamepad(joysticks[i], i);
		}
	SDL_free(joysticks);
}

void geUpdateControllerLastFrame(void) {
	for (int i = 0; i < _numGamePads; i++) {
		memcpy(_connectedGamepads[i].lastFrameAxis, _connectedGamepads[i].thisFrameAxis, sizeof(Uint8) * SDL_GAMEPAD_AXIS_COUNT);
		memcpy(_connectedGamepads[i].lastFrameButtons, _connectedGamepads[i].thisFrameButtons, sizeof(Uint8) * SDL_GAMEPAD_BUTTON_COUNT);
	}
}

bool geGamepadButtonJustReleased(const int padNum, const int button) {
	return _numGamePads > padNum && (!_connectedGamepads[padNum].thisFrameButtons[button] && _connectedGamepads[padNum].lastFrameButtons[button]);
}

bool geGamepadButtonJustPressedAnyPad(int button, int *pad) {
	for (size_t i = 0; i < (size_t)_numGamePads; i++) {
		if (geGamepadButtonJustPressed(i, button)) {
			*pad = i;
			return true;
		}
	}
	return false;
}

bool geGamepadButtonJustPressed(const int padNum, const int button) {
	return _numGamePads > padNum && (_connectedGamepads[padNum].thisFrameButtons[button] && !_connectedGamepads[padNum].lastFrameButtons[button]);
}
bool geGamepadButtonHeldDown(const int padNum, const int button) {
	return _numGamePads > padNum && (_connectedGamepads[padNum].lastFrameButtons[button] && _connectedGamepads[padNum].thisFrameButtons[button]);
}

int geGamepadLeftAxisXThisFrameMovement(const int padNum) {
	if (_numGamePads < padNum) {
		return 0;
	}
	GamePad *pad = &_connectedGamepads[padNum];
	// TODO this should be changed and inside joystick?
	int xInt = SDL_GAMEPAD_AXIS_LEFTX;
	return pad->thisFrameAxis[xInt] - pad->lastFrameAxis[xInt];
}

int geGamepadLeftAxisYThisFrameMovement(const int padNum) {
	if (_numGamePads < padNum) {
		return 0;
	}
	GamePad *pad = &_connectedGamepads[padNum];
	// TODO this should be changed and inside joystick?
	int yInt = SDL_GAMEPAD_AXIS_LEFTY;
	return pad->thisFrameAxis[yInt] - pad->lastFrameAxis[yInt];
}

float geGamepadLeftAxisXFloat(const int padNum) {
	if (_numGamePads < padNum) {
		return 0;
	}
	GamePad *pad = &_connectedGamepads[padNum];
	Sint16 xRaw = SDL_GetGamepadAxis(pad->Gamepad, SDL_GAMEPAD_AXIS_LEFTX);
	// Normalize to -1 to 1
	float x = xRaw / 32767.0f;
	if (fabs(x) < DEADZONE)
		x = 0;
	return x;
}
float geGamepadLeftAxisYFloat(const int padNum) {
	if (_numGamePads < padNum) {
		return 0;
	}
	GamePad *pad = &_connectedGamepads[padNum];
	Sint16 yRaw = SDL_GetGamepadAxis(pad->Gamepad, SDL_GAMEPAD_AXIS_LEFTY);
	float y = yRaw / 32767.0f;
	if (fabs(y) < DEADZONE)
		y = 0;
	return y;
}

void ShutdownJoystickSystem(void) {
	for (size_t i = 0; i < (size_t)_numGamePads; i++) {
		SDL_CloseGamepad(_connectedGamepads[i].Gamepad);
	}
}
