#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void SetCameraFollowTarget(float* x, float* y);
void SetCameraZoom(float zoom);
void SetCameraBounds(float x, float y);
float CameraGetX(void);
float CameraGetY(void);
float CameraGetWidth(void);
float CameraGetHeight(void);
/**
 * @brief  Sets the camera size, should likely be set to the window size.
 * @param x
 * @param y
 */
void SetCameraSize(float x, float y);
// Follows the target as needed, do not go past map bounds and stay at half width and update the projection
void UpdateCameraSystem(void);
#ifdef __cplusplus
}
#endif
