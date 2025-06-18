#pragma once
#ifdef __cplusplus
extern "C" {
#endif
extern float CameraX;
extern float CameraY;
extern float CameraZoom;
void SetCameraFollowTarget(float* x, float* y);
void SetCameraZoom(float zoom);
void SetCameraBounds(float x, float y);
void SetCameraSize(float x, float y);
void UpdateCamera(void);
#ifdef __cplusplus
}
#endif
