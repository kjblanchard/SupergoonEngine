#pragma once
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct Texture Texture;
typedef struct Shader Shader;
typedef struct Color Color;
struct sgGameObject;
typedef enum SpriteFlags {
	SpriteFlagVisible = 1 << 0,
	SpriteFlagDestroyed = 1 << 1,
} SpriteFlags;

typedef struct Sprite {
	float* parentX;
	float* parentY;
	Texture* Texture;
	RectangleF TextureSourceRect;
	RectangleF OffsetAndSizeRectF;
	float Scale;
	Shader* Shader;
	unsigned int Flags;
} Sprite;

Sprite* NewSprite(void);
// This doesn't get drawn during the libraries, useful for UI or something lke that.
Sprite* NewSpriteManual(void);
void DestroySprite(Sprite* sprite);
void DestroySpriteManual(Sprite* sprite);
void DrawSpriteManual(Sprite* sprite, RectangleF* dstRect, Color* color);
void DrawSpriteSystem();
void ShutdownSpriteSystem(void);
#ifdef __cplusplus
}
#endif
