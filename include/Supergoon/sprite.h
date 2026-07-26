#pragma once
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct Texture Texture;
typedef struct Shader Shader;
struct sgGameObject;
typedef enum SpriteFlags {
	SpriteFlagVisible = 1 << 0,
	SpriteFlagDestroyed = 1 << 1,
} SpriteFlags;

typedef struct Sprite {
	float* parentX;
	float* parentY;
	float prevParentX;
	float prevParentY;
	Texture* Texture;
	RectangleF TextureSourceRect;
	RectangleF OffsetAndSizeRectF;
	float Scale;
	Shader* Shader;
	Color DrawColor;
	unsigned int Flags;
} Sprite;

Sprite* NewSprite(void);
// This doesn't get drawn during the libraries, useful for UI or something lke that.
Sprite* NewSpriteManual(void);
void DestroySprite(Sprite* sprite);
void DestroySpriteManual(Sprite* sprite);
void DrawSpriteManual(Sprite* sprite, RectangleF* dstRect, Color* color, int camera);
void SnapshotSpritePositions(void);
void DrawSpriteSystem(void);
void ShutdownSpriteSystem(void);
#ifdef __cplusplus
}
#endif
