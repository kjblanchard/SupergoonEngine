#pragma once
#include <stdbool.h>
typedef union SDL_Event SDL_Event;

#ifdef __cplusplus
extern "C" {
#endif

void geHandleJoystickEvent(const SDL_Event* event);
void geInitializeJoysticks(void);
int geGamepadMaxPads(void);
void geUpdateControllerLastFrame(void);
int geGamepadLeftAxisXThisFrameMovement(const int padNum);
int geGamepadLeftAxisYThisFrameMovement(const int padNum);
float geGamepadLeftAxisXFloat(const int padNum);
float geGamepadLeftAxisYFloat(const int padNum);
bool geGamepadButtonJustPressedAnyPad(int button, int* pad);
bool geGamepadButtonJustReleased(const int padNum, const int button);
bool geGamepadButtonJustPressed(const int padNum, const int button);
bool geGamepadButtonHeldDown(const int padNum, const int button);

#ifdef __cplusplus
}
#endif
