/**
 * @file graphics.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief  Creating Textures and Drawing functions
 * @version 0.1
 * @date 2025-03-14
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

void DrawLine(float x1, float y1, float x2, float y2, float thickness, Color* color, int useCamera);
void DrawRect(RectangleF* rect, Color* color, int filled, int cameraOffset);
void InitializeGraphicsSystem(void);
void ShutdownGraphicsSystem(void);
void DrawStart(void);
void DrawUIStart(void);
void DrawEnd(void);
void GraphicsSetLogicalWorldSize(int width, int height);
int GraphicsGetTargetRefreshRate(void);
void GraphicsWindowResizeEvent(int width, int height);
void GraphicsUpdateFBOColor(Color* color);
Color GraphicsGetFBOColor(void);
void* GraphicsGetContextPtr(void);
extern void (*GraphicsPostFBODrawUIFunc)(void);
extern void (*GraphicsPostFBODrawDebugFunc)(void);

#ifdef __cplusplus
}
#endif
