#include <SDL3/SDL.h>
#include <Supergoon/Input/mouse.h>
#ifdef imgui
extern int _gameImagePosX;
extern int _gameImagePosY;
extern float _gameImageScale;
extern int _gameImageWidth;
extern int _gameImageHeight;
#endif
extern SDL_Renderer* _renderer;
extern int _logicalWidth;
extern int _logicalHeight;

void GetGameMousePos(float* x, float* y) {
#ifdef imgui
	float mousePosx, mousePosy;
	SDL_GetMouseState(&mousePosx, &mousePosy);
	int relX = mousePosx - _gameImagePosX;
	int relY = mousePosy - _gameImagePosY;
	if (relX >= 0 && relY >= 0 && relX < _gameImageWidth && relY < _gameImageHeight) {
		*x = relX / _gameImageScale;
		*y = relY / _gameImageScale;
	}
#else
	SDL_GetMouseState(x, y);
#endif
}

int IsMouseOverlap(int x, int y, int width, int height) {
#ifdef imgui
	float mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	int relX = mouseX - _gameImagePosX;
	int relY = mouseY - _gameImagePosY;
	if (relX < 0 || relY < 0 || relX >= _gameImageWidth || relY >= _gameImageHeight)
		return false;
	int logicX = relX / _gameImageScale;
	int logicY = relY / _gameImageScale;
	return (logicX >= x && logicX < x + width &&
			logicY >= y && logicY < y + height);
#else

	float mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	int windowW, windowH;
	SDL_GetRenderOutputSize(_renderer, &windowW, &windowH);
	int logicW = _logicalWidth;
	int logicH = _logicalHeight;
	int scaleX = windowW / logicW;
	int scaleY = windowH / logicH;
	int scale = (scaleX < scaleY) ? scaleX : scaleY;
	int viewportX = (windowW - (logicW * scale)) / 2;
	int viewportY = (windowH - (logicH * scale)) / 2;
	int logicMouseX = (mouseX - viewportX) / scale;
	int logicMouseY = (mouseY - viewportY) / scale;
	SDL_Point point = {logicMouseX, logicMouseY};
	SDL_Rect rect = {x, y, width, height};

	return SDL_PointInRect(&point, &rect);

#endif
}
