#include <SDL3/SDL_scancode.h>
#include <Supergoon/Input/keyboard.h>
#include <Supergoon/camera.h>
#include <Supergoon/state.h>
float cameraPos[3] = {0, 0, 0};
static float prevCameraPos[2] = {0, 0};
float cameraSize[3] = {0, 0, 0};
float BoundsX = 0;
float BoundsY = 0;
float CameraZoom = 1.0;
float* followX;
float* followY;

void UpdateCameraSystem(void) {
	prevCameraPos[0] = cameraPos[0];
	prevCameraPos[1] = cameraPos[1];
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
	cameraPos[0] = camX;
	cameraPos[1] = camY;
}

void SetCameraFollowTarget(float* x, float* y) {
	followX = x;
	followY = y;
	cameraPos[0] = 0;
	cameraPos[1] = 0;
	prevCameraPos[0] = 0;
	prevCameraPos[1] = 0;
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

static float getRenderCamX(void) {
	return prevCameraPos[0] + RenderAlpha * (cameraPos[0] - prevCameraPos[0]);
}

static float getRenderCamY(void) {
	return prevCameraPos[1] + RenderAlpha * (cameraPos[1] - prevCameraPos[1]);
}

float CameraGetX(void) {
	return SDL_floorf(getRenderCamX());
}

float CameraGetY(void) {
	return SDL_floorf(getRenderCamY());
}

void ResetCameraFollow(void) {
	followY = NULL;
	followX = NULL;
	cameraPos[0] = 0;
	cameraPos[1] = 0;
	prevCameraPos[0] = 0;
	prevCameraPos[1] = 0;
}

void CameraGetPositionHandle(float** x, float** y) {
	*x = &cameraPos[0];
	*y = &cameraPos[1];
}

void CameraGetFollow(float** x, float** y) {
	*x = followX;
	*y = followY;
}

float CameraGetRawX(void) { return cameraPos[0]; }
float CameraGetRawY(void) { return cameraPos[1]; }
float CameraGetSubPixelX(void) { float r = getRenderCamX(); return r - SDL_floorf(r); }
float CameraGetSubPixelY(void) { float r = getRenderCamY(); return r - SDL_floorf(r); }
float CameraGetWidth(void) { return cameraSize[0]; }
float CameraGetHeight(void) { return cameraSize[1]; }
