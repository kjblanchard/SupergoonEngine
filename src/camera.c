#include <SDL3/SDL.h>
#include <SupergoonEngine/camera.h>
float CameraX = 0;
float CameraY = 0;
float CamSizeX = 0;
float CamSizeY = 0;
float BoundsX = 0;
float BoundsY = 0;
float CameraZoom = 1.0f;
float* followX;
float* followY;

void UpdateCamera(void) {
	float viewWidth = CamSizeX / CameraZoom;
	float viewHeight = CamSizeY / CameraZoom;
	// Center camera on player
	float camX = CamSizeX;
	float camY = CamSizeY;
	if (followX || followY) {
		camX = *followX - viewWidth / 2.0f;
		camY = *followY - viewHeight / 2.0f;
	}
	if (camX < 0) camX = 0;
	if (camY < 0) camY = 0;
	if (camX > BoundsX - viewWidth) camX = BoundsX - viewWidth;
	if (camY > BoundsY - viewHeight) camY = BoundsY - viewHeight;
	CameraX = SDL_roundf(camX);
	CameraY = SDL_roundf(camY);
}

void SetCameraFollowTarget(float* x, float* y) {
	followX = x;
	followY = y;
}

void SetCameraBounds(float x, float y) {
	BoundsX = x;
	BoundsY = y;
}

void SetCameraSize(float x, float y) {
	CamSizeX = x;
	CamSizeY = y;
}

void SetCameraZoom(float zoom) {
	CameraZoom = zoom;
}
