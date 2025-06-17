#include <Supergoon/gameobject.h>
#include <Supergoon/graphics.h>
#include <Supergoon/sprite.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/sprite.h>
#include <SupergoonEngine/tools.h>
#include <limits.h>
#include <stdlib.h>

static size_t _firstSpriteHole = NO_HOLE;  // CHANGED: Use -1 as sentinel for "no hole"
static size_t _numSprites = 0;
static size_t _sizeSprites = 0;
static Sprite** _sprites;

static Sprite* getFreeSprite(void) {
	if (_firstSpriteHole == (size_t)-1) {  // CHANGED: Check sentinel value
		int oldSize = _sizeSprites;
		RESIZE_ARRAY(_sprites, _numSprites, _sizeSprites, Sprite*);
		if (oldSize < _sizeSprites) {
			for (size_t i = oldSize; i < _sizeSprites; i++) {
				_sprites[i] = calloc(1, sizeof(Sprite));
			}
		}
		return _sprites[_numSprites++];
	}

	// Reuse a destroyed sprite
	Sprite* returnSprite = _sprites[_firstSpriteHole];

	// Find next hole (if any)
	size_t nextHole = (size_t)-1;
	for (size_t i = _firstSpriteHole + 1; i < _numSprites; i++) {
		if (_sprites[i]->Flags & SpriteFlagDestroyed) {
			nextHole = i;
			break;
		}
	}
	_firstSpriteHole = nextHole;
	return returnSprite;
}

void InitializeSpriteSystem(void) {
	// RESIZE_ARRAY(_sprites, _sizeSprites, Sprite*);
	// _firstSpriteHole = (size_t)-1;	// CHANGED: Initialize properly
}

Sprite* NewSprite(void) {
	// SpriteHandle handle = getFreeSprite();
	Sprite* sprite = getFreeSprite();
	sprite->Parent = 0;
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
		sprite->Parent = NULL;
		sprite->Texture = NULL;
		sprite->Flags = SpriteFlagDestroyed;

		// CHANGED: Update hole tracker properly
		if (_firstSpriteHole == (size_t)-1 || i < _firstSpriteHole) {
			_firstSpriteHole = i;
		}

		return;
	}
}

void DrawSpriteSystem(void) {
	for (size_t i = 0; i < _numSprites; i++) {
		Sprite* sprite = _sprites[i];
		if (!sprite || !sprite->Texture || !(sprite->Flags & SpriteFlagVisible)) {
			continue;
		}

		float interpX = 0;
		float interpY = 0;

		if (sprite->Parent) {
			interpX = sprite->Parent->X;
			interpY = sprite->Parent->Y;
		}

		RectangleF dst = sprite->OffsetAndSizeRectF;
		dst.x = interpX + dst.x;
		dst.y = interpY + dst.y;
		dst.x = SDL_roundf(dst.x);
		dst.y = SDL_roundf(dst.y);

		DrawTexture(sprite->Texture, &dst, &sprite->TextureSourceRect);
	}
}

void ShutdownSpriteSystem(void) {
	for (size_t i = 0; i < _numSprites; i++) {
		UnloadTexture(_sprites[i]->Texture);
	}
	SDL_free(_sprites);
	_sprites = NULL;
	_numSprites = 0;
	_sizeSprites = 4;
	_firstSpriteHole = (size_t)-1;	// Reset on shutdown
}
