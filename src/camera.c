#include <SDL3/SDL_scancode.h>
#include <Supergoon/Input/keyboard.h>
#include <Supergoon/camera.h>
double cameraPos[3] = {0, 0, 0};
double cameraSize[3] = {0, 0, 0};
double BoundsX = 0;
double BoundsY = 0;
double CameraZoom = 1.0;
float* followX;
float* followY;

void UpdateCameraSystem(void) {
	double viewWidth = 480;
	double viewHeight = 270;
	double camX = 0;
	double camY = 0;
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
	/* cameraPos[0] = floorf(camX); */
	/* cameraPos[1] = floor(camY); */
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
double CameraGetX(void) {
	return cameraPos[0];
}
double CameraGetY(void) {
	return cameraPos[1];
}

void ResetCameraFollow(void){
	followY = NULL;
	followX = NULL;
	cameraPos[0] = 0;
	cameraPos[1] = 0;


}

float CameraGetWidth(void) { return cameraSize[0]; }
float CameraGetHeight(void) { return cameraSize[1]; }
