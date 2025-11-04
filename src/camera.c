#include <Supergoon/camera.h>
#include <cglm/vec3.h>
#include <math.h>
vec3 cameraPos = {0, 0, 0};
float CamSizeX = 0;
float CamSizeY = 0;
float BoundsX = 0;
float BoundsY = 0;
float CameraZoom = 1.0f;
float* followX;
float* followY;

void UpdateCameraSystem(void) {
	double viewWidth = CamSizeX;
	double viewHeight = CamSizeY;
	double camX = 0;
	double camY = 0;
	if (followX && followY) {
		camX = round(*followX) - viewWidth / 2.0f;
		camY = round(*followY) - viewHeight / 2.0f;
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
	cameraPos[0] = roundf(camX);
	cameraPos[1] = roundf(camY);
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
	CamSizeX = x;
	CamSizeY = y;
}

void SetCameraZoom(float zoom) {
	CameraZoom = zoom;
}
float CameraGetX(void) {
	return cameraPos[0];
}
float CameraGetY(void) {
	return cameraPos[1];
}
