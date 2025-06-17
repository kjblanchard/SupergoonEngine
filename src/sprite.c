// #include <Supergoon/gameobject.h>
// #include <Supergoon/graphics.h>
// #include <Supergoon/sprite.h>
// #include <SupergoonEngine/sprite.h>
// #include <SupergoonEngine/tools.h>
// #include <stdlib.h>

// static size_t _firstSpriteHole = 0;
// static size_t _numSprites = 0;
// static size_t _sizeSprites = 4;
// static Sprite* _sprites;
// // TODO turn this into a macro to reuse easily.
// static Sprite* getFreeSprite(void) {
// 	if (!_firstSpriteHole) {
// 		RESIZE_ARRAY_FULL(_sprites, _numSprites, _sizeSprites, Sprite);
// 		// if (_numSprites + 1 > _sizeSprites / 2) {
// 		// 	RESIZE_ARRAY(_sprites, _sizeSprites, Sprite);
// 		// }
// 		return &_sprites[_numSprites++];
// 	}
// 	Sprite* returnSprite = &_sprites[_firstSpriteHole];
// 	size_t nextHole = 0;
// 	for (size_t i = _firstSpriteHole + 1; i < _numSprites; i++) {
// 		if (_sprites[i].Flags & SpriteFlagDestroyed) {
// 			nextHole = i;
// 			break;
// 		}
// 	}
// 	_firstSpriteHole = nextHole;
// 	return returnSprite;
// }

// void InitializeSpriteSystem(void) {
// 	RESIZE_ARRAY(_sprites, _sizeSprites, Sprite);
// }

// Sprite* NewSprite(void) {
// 	Sprite* sprite = getFreeSprite();
// 	sprite->Parent = NULL;
// 	sprite->Texture = NULL;
// 	sprite->Flags = 0;
// 	sprite->TextureSourceRect = (RectangleF){0, 0, 0, 0};
// 	sprite->OffsetAndSizeRectF = (RectangleF){0, 0, 0, 0};
// 	return sprite;
// }

// void DestroySprite(Sprite* sprite) {
// 	if (!sprite) {
// 		sgLogWarn("Trying to destroy a null sprite!");
// 	}
// 	for (size_t i = 0; i < _numSprites; i++) {
// 		if (sprite != &_sprites[i]) {
// 			continue;
// 		}
// 		UnloadTexture(sprite->Texture);
// 		sprite->Parent = NULL;
// 		sprite->Texture = NULL;
// 		sprite->Flags = SpriteFlagDestroyed;
// 		_firstSpriteHole = _firstSpriteHole < i ? _firstSpriteHole : i;
// 		return;
// 	}
// }

// void DrawSpriteSystem(void) {
// 	for (size_t i = 0; i < _numSprites; i++) {
// 		Sprite* sprite = &_sprites[i];
// 		if (!sprite || !sprite->Texture || !(sprite->Flags & SpriteFlagVisible)) {
// 			continue;
// 		}

// 		float interpX = 0;
// 		float interpY = 0;

// 		if (sprite->Parent) {
// 			interpX = sprite->Parent->X;
// 			interpY = sprite->Parent->Y;
// 			// interpX = sprite->Parent->pX + alpha * (sprite->Parent->X - sprite->Parent->pX);
// 			// interpY = sprite->Parent->pY + alpha * (sprite->Parent->Y - sprite->Parent->pY);
// 		}

// 		RectangleF dst = sprite->OffsetAndSizeRectF;
// 		dst.x = interpX + dst.x;
// 		dst.y = interpY + dst.y;
// 		// dst.x = (int)SDL_roundf(dst.x);
// 		// dst.y = (int)SDL_roundf(dst.y);
// 		dst.x = SDL_roundf(dst.x);
// 		dst.y = SDL_roundf(dst.y);

// 		DrawTexture(sprite->Texture, &dst, &sprite->TextureSourceRect);
// 	}
// }

// void ShutdownSpriteSystem(void) {
// 	for (size_t i = 0; i < _numSprites; i++) {
// 		UnloadTexture(_sprites[i].Texture);
// 	}
// 	SDL_free(_sprites);
// }

#include <Supergoon/gameobject.h>
#include <Supergoon/graphics.h>
#include <Supergoon/sprite.h>
#include <SupergoonEngine/sprite.h>
#include <SupergoonEngine/tools.h>
#include <stdlib.h>

static size_t _firstSpriteHole = (size_t)-1;  // CHANGED: Use -1 as sentinel for "no hole"
static size_t _numSprites = 0;
static size_t _sizeSprites = 4;
static Sprite* _sprites;

static Sprite* getFreeSprite(void) {
	if (_firstSpriteHole == (size_t)-1) {  // CHANGED: Check sentinel value
		RESIZE_ARRAY_FULL(_sprites, _numSprites, _sizeSprites, Sprite);
		return &_sprites[_numSprites++];
	}

	// Reuse a destroyed sprite
	Sprite* returnSprite = &_sprites[_firstSpriteHole];

	// Find next hole (if any)
	size_t nextHole = (size_t)-1;
	for (size_t i = _firstSpriteHole + 1; i < _numSprites; i++) {
		if (_sprites[i].Flags & SpriteFlagDestroyed) {
			nextHole = i;
			break;
		}
	}
	_firstSpriteHole = nextHole;
	return returnSprite;
}

void InitializeSpriteSystem(void) {
	RESIZE_ARRAY(_sprites, _sizeSprites, Sprite);
	_firstSpriteHole = (size_t)-1;	// CHANGED: Initialize properly
}

Sprite* NewSprite(void) {
	Sprite* sprite = getFreeSprite();
	sprite->Parent = NULL;
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
		if (sprite != &_sprites[i]) {
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
		Sprite* sprite = &_sprites[i];
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
		UnloadTexture(_sprites[i].Texture);
	}
	SDL_free(_sprites);
	_sprites = NULL;
	_numSprites = 0;
	_sizeSprites = 4;
	_firstSpriteHole = (size_t)-1;	// Reset on shutdown
}
