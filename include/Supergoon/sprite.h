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
typedef enum SpriteFlags {
	SpriteFlagVisible = 1 << 0,
	SpriteFlagDestroyed = 1 << 1,
	SpriteFlagManual = 1 << 2,
} SpriteFlags;
// Parentx/y allows for this to be attached to some "gameobject" and it will "follow" that thing if its set
// Also allows for interpolating over that parents movement for frame perfect movement
// Offset/size handles it's actual offset from the gameobject and it's size
typedef struct Sprite {
	float ParentX;
	float ParentY;
	float PrevParentX;
	float PrevParentY;
	Texture* Texture;
	RectangleF TextureSourceRect;
	RectangleF OffsetAndSizeRectF;
	float Scale;
	Shader* Shader;
	Color DrawColor;
	unsigned int Flags;
  unsigned int Layer;
  unsigned int Priority;
} Sprite;
Sprite* SpriteNew(void);
void SpriteSetVisible(Sprite* s, bool v);
void SpriteSetManual(Sprite* s, bool v);
void SpriteDestroy(Sprite* sprite);
void SpriteDrawManual(Sprite* sprite, RectangleF* dstRect, Color* color, int camera);
void SpriteSystemUpdate(void);
void SpriteSystemDraw(void);
void SpriteSystemShutdown(void);
#ifdef __cplusplus
}
#endif
