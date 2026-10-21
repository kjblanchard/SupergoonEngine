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
void DrawRect(const RectangleF* rect, Color* color, int filled, int cameraOffset);
void InitializeGraphicsSystem(void);
void ShutdownGraphicsSystem(void);
void DrawStart(void);
void DrawUIStart(void);
void DrawEnd(void);
void GraphicsSetLogicalWorldSize(int width, int height);
void GraphicsWindowResizeEvent(int width, int height);
void GraphicsUpdateFBOColor(Color* color);
Color GraphicsGetFBOColor(void);
void* GraphicsGetContextPtr(void);
void GraphicsSetPremultipliedBlend(int premultiplied);
// Scale UI/text are rasterized at; same as GraphicsGetGameScale.
int GraphicsGetUIScale(void);
// SDL backend only.  Integer scale the game is presented at.  Without imgui this
// is the window scale; with imgui it follows the "Game" debug window.
int GraphicsGetGameScale(void);
// SDL backend only.  Largest integer scale of the logical size that fits the OS window.
int GraphicsGetWindowScale(void);
// SDL backend only.  Requests a new game scale (imgui builds); applied at the
// next DrawStart, which rebuilds the scaled targets and re-renders all text.
void GraphicsRequestGameScale(int scale);

#ifdef __cplusplus
}
#endif
