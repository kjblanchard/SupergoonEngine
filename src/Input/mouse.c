#include <SDL3/SDL.h>
#include <Supergoon/Input/mouse.h>
#include <SupergoonEngine/window.h>

void GetGameMousePos(float* x, float* y) {
	float mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	int relX = mouseX - _gameImagePosX;
	int relY = mouseY - _gameImagePosY;
	if (relX >= 0 && relY >= 0 && relX < _gameImageWidth && relY < _gameImageHeight) {
		*x = relX / _gameImageScale;
		*y = relY / _gameImageScale;
	} else {
		*x = -1;
		*y = -1;
	}
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
