#pragma once
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct GameObject GameObject;
typedef struct Texture Texture;
typedef struct Shader Shader;
struct GameObject;
typedef enum SpriteFlags {
	SpriteFlagVisible = 1 << 0,
	SpriteFlagDestroyed = 1 << 1,
} SpriteFlags;
typedef struct Sprite {
	GameObject* Parent;
	Texture* Texture;
	RectangleF TextureSourceRect;
	RectangleF OffsetAndSizeRectF;
	Shader* Shader;
	SpriteFlags Flags;
} Sprite;

Sprite* NewSprite(void);
void DestroySprite(Sprite* sprite);
void DrawSpriteSystem();
void ShutdownSpriteSystem(void);
#ifdef __cplusplus
}
#endif
