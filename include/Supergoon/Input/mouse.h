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
// Checks to see if the mouse overlapped here, in game coords.
int IsMouseOverlapRect(float x, float y, float width, float height);
#ifdef imgui
// Where the game is drawn inside the imgui Game window, in window points.
// Set each frame by the Game window; hovered=0 makes the mouse read as outside the game.
void MouseSetGameViewRect(float x, float y, float width, float height, int hovered);
#endif
int IsMouseButtonDown(int button);
int IsMouseButtonJustPressed(int button);
int IsMouseButtonJustReleased(int button);
// Functions in mouce.c
void UpdateMouseSystem(void);
void UpdateTouchSystem(void);

#ifdef __cplusplus
}
#endif
