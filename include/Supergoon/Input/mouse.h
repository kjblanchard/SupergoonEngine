#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef enum MouseButtons {
	MouseButtonsLeftClick,
	MouseButtonsRightClick,
	MouseButtonsMiddleClick,
} MouseButtons;

// Converts a point in SDL window points to game coords, returns 0 and sets -1 if outside the game
int WindowPointToGamePos(float winX, float winY, float* x, float* y);
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

#ifdef __cplusplus
}
#endif
