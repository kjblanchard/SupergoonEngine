#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/camera.h>
#include <Supergoon/gameobject.h>
#include <Supergoon/sprite.h>
#include <Supergoon/tools.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static size_t _firstSpriteHole = NO_HOLE;
static size_t _numSprites = 0;
static size_t _sizeSprites = 0;
static Sprite** _sprites;

static Sprite* getFreeSprite(void) {
	if (_firstSpriteHole == NO_HOLE) {
		RESIZE_ARRAY_PTR_ALLOC(_sprites, _numSprites, _sizeSprites, Sprite);
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

Sprite* NewSprite(void) {
	Sprite* sprite = getFreeSprite();
	sprite->Texture = NULL;
	sprite->Flags = 0;
	sprite->TextureSourceRect = (RectangleF){0, 0, 0, 0};
	sprite->OffsetAndSizeRectF = (RectangleF){0, 0, 0, 0};
	return sprite;
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
		UnloadTexture(sprite->Texture);
		sprite->Texture = NULL;
		sprite->Flags = SpriteFlagDestroyed;
		if (_firstSpriteHole == NO_HOLE || i < _firstSpriteHole) {
			_firstSpriteHole = i;
		}
		return;
	}
}

void DrawSpriteSystem(void) {
	RectangleF dst = {0, 0, 0, 0};
	for (size_t i = 0; i < _numSprites; i++) {
		Sprite* sprite = _sprites[i];
		if (!sprite || !sprite->Texture || NO_FLAGS(sprite->Flags, SpriteFlagVisible) || HAS_ALL_FLAGS(sprite->Flags, SpriteFlagUI)) {
			// if (!sprite || !sprite->Texture || NO_FLAGS(sprite->Flags, SpriteFlagVisible)  !(sprite->Flags & SpriteFlagVisible) || sprite->Flags & SpriteFlagUI) {
			continue;
		}
		float globalX = 0;
		float globalY = 0;
		dst.x = roundf(globalX + sprite->OffsetAndSizeRectF.x - CameraX);
		dst.y = roundf(globalY + sprite->OffsetAndSizeRectF.y - CameraY);

		dst.w = sprite->OffsetAndSizeRectF.w;
		dst.h = sprite->OffsetAndSizeRectF.h;
		DrawTexture(sprite->Texture, &dst, &sprite->TextureSourceRect);
	}
}

void ShutdownSpriteSystem(void) {
	for (size_t i = 0; i < _sizeSprites; i++) {
		UnloadTexture(_sprites[i]->Texture);
	}
	free(_sprites);
	_sprites = NULL;
	_numSprites = 0;
	_sizeSprites = 4;
	_firstSpriteHole = (size_t)-1;	// Reset on shutdown
}
