#pragma once
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
	Texture* Texture;
	RectangleF TextureSourceRect;
	RectangleF OffsetAndSizeRectF;
	float Scale;
	Shader* Shader;
	unsigned int Flags;
} Sprite;

Sprite* NewSprite(void);
void DestroySprite(Sprite* sprite);
void DrawSpriteSystem();
void ShutdownSpriteSystem(void);
#ifdef __cplusplus
}
#endif
