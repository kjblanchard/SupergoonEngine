#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/camera.h>
#include <Supergoon/sprite.h>
#include <Supergoon/tools.h>
#include <stdlib.h>

static size_t _firstSpriteHole = NO_HOLE;
static size_t _numSprites = 0;
static size_t _sizeSprites = 0;
static Sprite** _sprites;

static Sprite* getFreeSprite(void) {
	if (_firstSpriteHole == NO_HOLE) {
		RESIZE_ARRAY_PTR_ALLOC(_sprites, _numSprites, _sizeSprites, Sprite, 0);
		return _sprites[_numSprites++];
	}
	Sprite* returnSprite = _sprites[_firstSpriteHole];
	size_t nextHole = NO_HOLE;
	for (size_t i = _firstSpriteHole + 1; i < _numSprites; i++) {
		if (_sprites[i]->Flags & SpriteFlagDestroyed) {
			nextHole = i;
			break;
		}
	}
	_firstSpriteHole = nextHole;
	return returnSprite;
}

void initSprite(Sprite* sprite) {
	sprite->parentX = NULL;
	sprite->parentY = NULL;
	sprite->Texture = NULL;
	sprite->Shader = NULL;
	sprite->Scale = 1.0f;
	sprite->Flags = 0;
	sprite->TextureSourceRect = (RectangleF){0, 0, 0, 0};
	sprite->OffsetAndSizeRectF = (RectangleF){0, 0, 0, 0};
}

Sprite* NewSprite(void) {
	Sprite* sprite = getFreeSprite();
	initSprite(sprite);
	return sprite;
}
Sprite* NewSpriteManual(void) {
	Sprite* sprite = malloc(sizeof(*sprite));
	initSprite(sprite);
	return sprite;
}

void destroySprite(Sprite* sprite) {
	// If we are using the default shader, this breaks, so prevent it from destroying shader if so.
	if (GetDefaultShader() != sprite->Shader) ShaderDestroy(sprite->Shader);
	TextureDestroy(sprite->Texture);
	sprite->Texture = NULL;
}

void DestroySpriteManual(Sprite* sprite) {
	destroySprite(sprite);
	free(sprite);
}

void DestroySprite(Sprite* sprite) {
	if (!sprite) {
		sgLogWarn("Trying to destroy a null sprite!");
		return;
	}
	for (size_t i = 0; i < _numSprites; i++) {
		if (sprite != _sprites[i]) {
			continue;
		}
		destroySprite(sprite);
		sprite->Flags = SpriteFlagDestroyed;
		if (_firstSpriteHole == NO_HOLE || i < _firstSpriteHole) {
			_firstSpriteHole = i;
		}
		return;
	}
}

void DrawSpriteManual(Sprite* sprite, RectangleF* dstRect, Color* color) {
	if (!sprite || !sprite->Texture || !(sprite->Flags & SpriteFlagVisible)) {
		return;
	}
	dstRect->x = sprite->parentX ? *sprite->parentX + sprite->OffsetAndSizeRectF.x : sprite->OffsetAndSizeRectF.x;
	dstRect->y = sprite->parentY ? *sprite->parentY + sprite->OffsetAndSizeRectF.y : sprite->OffsetAndSizeRectF.y;
	DrawTexture(sprite->Texture, sprite->Shader, dstRect, &sprite->TextureSourceRect, true, sprite->Scale, false, color);
}

void DrawSpriteSystem(void) {
	RectangleF dst = (RectangleF){0, 0, 0, 0};
	Color color = {255, 255, 255, 255};
	for (size_t i = 0; i < _numSprites; i++) {
		Sprite* sprite = _sprites[i];
		dst.w = sprite->OffsetAndSizeRectF.w;
		dst.h = sprite->OffsetAndSizeRectF.h;
		DrawSpriteManual(sprite, &dst, &color);
	}
}

void ShutdownSpriteSystem(void) {
	for (size_t i = 0; i < _sizeSprites; i++) {
		DestroySprite(_sprites[i]);
		free(_sprites[i]);
	}
	free(_sprites);
	_sprites = NULL;
	_numSprites = 0;
	_sizeSprites = 4;
	_firstSpriteHole = (size_t)-1;	// Reset on shutdown
}
