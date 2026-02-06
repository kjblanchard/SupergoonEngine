#include <SDL3/SDL_scancode.h>
#include <Supergoon/Input/keyboard.h>
#include <Supergoon/camera.h>
float cameraPos[3] = {0, 0, 0};
float cameraSize[3] = {0, 0, 0};
float BoundsX = 0;
float BoundsY = 0;
float CameraZoom = 1.0;
float* followX;
float* followY;

void UpdateCameraSystem(void) {
	float viewWidth = 480;
	float viewHeight = 270;
	float camX = 0;
	float camY = 0;
	if (followX && followY) {
		camX = (*followX) - viewWidth / 2.0f;
		camY = (*followY) - viewHeight / 2.0f;
	}
	// If map is smaller than screen, camera should be pinned to top-left
	if (BoundsX <= viewWidth)
		camX = 0;
	else if (camX < 0)
		camX = 0;
	else if (camX > BoundsX - viewWidth)
		camX = BoundsX - viewWidth;

	if (BoundsY <= viewHeight)
		camY = 0;
	else if (camY < 0)
		camY = 0;
	else if (camY > BoundsY - viewHeight)
		camY = BoundsY - viewHeight;
	/* cameraPos[0] = SDL_floorf(camX); */
	/* cameraPos[1] = SDL_floorf(camY); */
	cameraPos[0] = (camX);
	cameraPos[1] = (camY);
}

void SetCameraFollowTarget(float* x, float* y) {
	followX = x;
	followY = y;
	cameraPos[0] = 0;
	cameraPos[1] = 0;
}

void SetCameraBounds(float x, float y) {
	BoundsX = x;
	BoundsY = y;
}

void SetCameraSize(float x, float y) {
	cameraSize[0] = x;
	cameraSize[1] = y;
}

void SetCameraZoom(float zoom) {
	CameraZoom = zoom;
}

float CameraGetX(void) {
	/* return SDL_roundf(cameraPos[0]); */
	return (cameraPos[0]);
	/* return SDL_roundf(cameraPos[0]); */
}

float CameraGetY(void) {
	/* return SDL_roundf(cameraPos[1]); */
	return (cameraPos[1]);
}

void ResetCameraFollow(void) {
	followY = NULL;
	followX = NULL;
	cameraPos[0] = 0;
	cameraPos[1] = 0;
}

float CameraGetWidth(void) { return cameraSize[0]; }
float CameraGetHeight(void) { return cameraSize[1]; }
