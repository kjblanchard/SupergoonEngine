#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/camera.h>
#include <Supergoon/sprite.h>
#include <sgtools/tools.h>
#include <string.h>

static size_t numSprites = 0;
static size_t sizeSprites = 0;
static Sprite** sprites;

static void initSprite(Sprite* sprite) {
	sprite->ParentX = 0;
	sprite->ParentY = 0;
	sprite->PrevParentX = 0;
	sprite->PrevParentY = 0;
	sprite->Texture = NULL;
	sprite->Shader = NULL;
	sprite->Scale = 1.0f;
	sprite->DrawColor = (Color){255, 255, 255, 255};
	sprite->Flags = 0;
	sprite->Layer = 0;
	sprite->Priority = 0;
	sprite->TextureSourceRect = (RectangleF){0, 0, 0, 0};
	sprite->OffsetAndSizeRectF = (RectangleF){0, 0, 0, 0};
}

Sprite* SpriteNew(void) {
	ArrayResizeIfNeeded((void**)&sprites, numSprites + 1, &sizeSprites, sizeof(Sprite*));
	Sprite* sprite = malloc(sizeof(*sprite));
	initSprite(sprite);
	sprites[numSprites] = sprite;
	++numSprites;
	return sprite;
}

static void destroySprite(Sprite* sprite) {
	// If we are using the default shader, this breaks, so prevent it from destroying shader if so.
	// if (GetDefaultShader() != sprite->Shader) ShaderDestroy(sprite->Shader);
	// TextureDestroy(sprite->Texture);
	// sprite->Texture = NULL;
	// sprite->Shader = NULL;
}

void SpriteDestroy(Sprite* sprite) {
	// if (!sprite) {
	// 	sgLogDebug("Trying to destroy a null sprite!");
	// 	return;
	// }
	// for (size_t i = 0; i < numSprites; i++) {
	// 	if (sprite != sprites[i]) {
	// 		continue;
	// 	}
	// 	destroySprite(sprite);
	// 	sprite->Flags = SpriteFlagDestroyed;
	// 	if (_firstSpriteHole == NO_HOLE || i < _firstSpriteHole) {
	// 		_firstSpriteHole = i;
	// 	}
	// 	return;
	// }
}

void SpriteDrawManual(Sprite* sprite, RectangleF* dstRect, Color* color, int camera) {
	if (!sprite || !sprite->Texture || !(sprite->Flags & SpriteFlagVisible)) {
		return;
	}
	RectangleF dst = *dstRect;

	if (camera && sprite->ParentX) {
		float interpX = sprite->PrevParentX + CameraGetInterpolationAlpha() * (sprite->ParentX - sprite->PrevParentX);
		dst.x = interpX + sprite->OffsetAndSizeRectF.x;
	} else {
		dst.x = sprite->ParentX ? sprite->ParentX + sprite->OffsetAndSizeRectF.x : sprite->OffsetAndSizeRectF.x;
	}
	if (camera && sprite->ParentY) {
		float interpY = sprite->PrevParentY + CameraGetInterpolationAlpha() * (sprite->ParentY - sprite->PrevParentY);
		dst.y = interpY + sprite->OffsetAndSizeRectF.y;
	} else {
		dst.y = sprite->ParentY ? sprite->ParentY + sprite->OffsetAndSizeRectF.y : sprite->OffsetAndSizeRectF.y;
	}
	DrawTexture(sprite->Texture, sprite->Shader, &dst, &sprite->TextureSourceRect, camera, sprite->Scale, false, color);
}

void SpriteSystemUpdate(void) {
	for (size_t i = 0; i < numSprites; i++) {
		Sprite* sprite = sprites[i];
		if (sprite->Flags & SpriteFlagDestroyed) continue;
		if (sprite->ParentX) sprite->PrevParentX = sprite->ParentX;
		if (sprite->ParentY) sprite->PrevParentY = sprite->ParentY;
	}
}

void SpriteSystemDraw(void) {
	RectangleF dst = (RectangleF){0, 0, 0, 0};
	for (size_t i = 0; i < numSprites; i++) {
		Sprite* sprite = sprites[i];
		if (sprite->Flags & SpriteFlagManual || !(sprite->Flags & SpriteFlagVisible)) {
			continue;
		}
		dst.w = sprite->OffsetAndSizeRectF.w;
		dst.h = sprite->OffsetAndSizeRectF.h;
		SpriteDrawManual(sprite, &dst, &sprite->DrawColor, true);
	}
}

void SpriteSystemShutdown(void) {
	// for (size_t i = 0; i < sizeSprites; i++) {
	// 	SpriteDestroy(sprites[i]);
	// 	free(sprites[i]);
	// }
	// free(sprites);
	// sprites = NULL;
	// numSprites = 0;
	// sizeSprites = 4;
}

void SpriteSetManual(Sprite* s, bool v) {
	if (v) {
		s->Flags |= SpriteFlagManual;
	} else {
		s->Flags &= ~SpriteFlagManual;
	}
}

void SpriteSetVisible(Sprite* s, bool v) {
	if (v) {
		s->Flags |= SpriteFlagVisible;
	} else {
		s->Flags &= ~SpriteFlagVisible;
	}
}
