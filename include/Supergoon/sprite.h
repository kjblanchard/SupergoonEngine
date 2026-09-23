/**
 * @file sprite.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief sprites
 * @version 0.1
 * @date 9-8-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct Texture Texture;
typedef struct Shader Shader;
// Parentx/y is the base location that should be updated every frame
// Offset/size handles it's actual offset from the parent.
// Caller should handle memory of the sprite from spritenew by calling spritedestroy.
// All sprites created with spritenew are drawn automatically, unless they are set to manual
typedef struct Sprite {
	RectangleF Location;
	float PreviousX;
	float PreviousY;
	Texture* Texture;
	RectangleF TextureSourceRect;
	float Scale;
	Shader* Shader;
	Color DrawColor;
	unsigned int Visible : 1;
	unsigned int Manual : 1;
} Sprite;
Sprite* SpriteNew(void);
void SpriteDestroy(Sprite* sprite);
void SpriteDrawManual(Sprite* sprite, Color* color, int camera);
void SpriteSystemUpdate(void);
void SpriteSystemDraw(void);
void SpriteSystemShutdown(void);
#ifdef __cplusplus
}
#endif
