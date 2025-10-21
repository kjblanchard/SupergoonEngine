#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef enum MouseButtons {
	MouseButtonsLeftClick,
	MouseButtonsRightClick,
	MouseButtonsMiddleClick,
} MouseButtons;

// Gets the mouse position in the game window
void GetGameMousePos(float* x, float* y);
// Checks to see if the mouse overlapped here.
int IsMouseOverlapRect(int x, int y, int width, int height);
int IsMouseButtonDown(int button);
int IsMouseButtonJustPressed(int button);
int IsMouseButtonJustReleased(int button);
// Functions in mouce.c
void UpdateMouseSystem(void);
void UpdateTouchSystem(void);

#ifdef __cplusplus
}
#endif
