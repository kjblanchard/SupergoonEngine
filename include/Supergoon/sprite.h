#pragma once
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct SDL_Texture Texture;
struct GameObject;
typedef enum SpriteFlags {
	SpriteFlagVisible = 1 << 0,
	SpriteFlagDestroyed = 1 << 1,
	SpriteFlagUI = 1 << 2,
} SpriteFlags;
typedef struct Sprite {
	struct GameObject* Parent;
	Texture* Texture;
	RectangleF TextureSourceRect;
	RectangleF OffsetAndSizeRectF;
	SpriteFlags Flags;
} Sprite;

Sprite* NewSprite(void);
void DestroySprite(Sprite* sprite);
void DrawSpriteSystem();
void ShutdownSpriteSystem(void);
#ifdef __cplusplus
}
#endif
