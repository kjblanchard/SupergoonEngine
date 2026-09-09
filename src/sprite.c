#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/camera.h>
#include <Supergoon/sprite.h>
#include <sgtools/tools.h>
#include <string.h>

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

static void initSprite(Sprite* sprite) {
	sprite->ParentX = NULL;
	sprite->ParentY = NULL;
	sprite->PrevParentX = 0;
	sprite->PrevParentY = 0;
	sprite->Texture = NULL;
	sprite->Shader = NULL;
	sprite->Scale = 1.0f;
	sprite->DrawColor = (Color){255, 255, 255, 255};
	sprite->Flags = 0;
	sprite->TextureSourceRect = (RectangleF){0, 0, 0, 0};
	sprite->OffsetAndSizeRectF = (RectangleF){0, 0, 0, 0};
}

Sprite* SpriteNew(void) {
	Sprite* sprite = getFreeSprite();
	initSprite(sprite);
	return sprite;
}

static void destroySprite(Sprite* sprite) {
	// If we are using the default shader, this breaks, so prevent it from destroying shader if so.
	if (GetDefaultShader() != sprite->Shader) ShaderDestroy(sprite->Shader);
	TextureDestroy(sprite->Texture);
	sprite->Texture = NULL;
	sprite->ParentX = NULL;
	sprite->ParentY = NULL;
}

void SpriteDestroy(Sprite* sprite) {
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

void SpriteDrawManual(Sprite* sprite, RectangleF* dstRect, Color* color, int camera) {
	if (!sprite || !sprite->Texture || !(sprite->Flags & SpriteFlagVisible)) {
		return;
	}
	if (camera && sprite->ParentX) {
		float interpX = sprite->PrevParentX + CameraGetInterpolationAlpha() * (*sprite->ParentX - sprite->PrevParentX);
		dstRect->x = interpX + sprite->OffsetAndSizeRectF.x;
	} else {
		dstRect->x = sprite->ParentX ? *sprite->ParentX + sprite->OffsetAndSizeRectF.x : sprite->OffsetAndSizeRectF.x;
	}
	if (camera && sprite->ParentY) {
		float interpY = sprite->PrevParentY + CameraGetInterpolationAlpha() * (*sprite->ParentY - sprite->PrevParentY);
		dstRect->y = interpY + sprite->OffsetAndSizeRectF.y;
	} else {
		dstRect->y = sprite->ParentY ? *sprite->ParentY + sprite->OffsetAndSizeRectF.y : sprite->OffsetAndSizeRectF.y;
	}
	DrawTexture(sprite->Texture, sprite->Shader, dstRect, &sprite->TextureSourceRect, camera, sprite->Scale, false, color);
}

void SpriteSystemUpdate(void) {
	for (size_t i = 0; i < _numSprites; i++) {
		Sprite* sprite = _sprites[i];
		if (sprite->Flags & SpriteFlagDestroyed) continue;
		if (sprite->ParentX) sprite->PrevParentX = *sprite->ParentX;
		if (sprite->ParentY) sprite->PrevParentY = *sprite->ParentY;
	}
}

void SpriteSystemDraw(void) {
	RectangleF dst = (RectangleF){0, 0, 0, 0};
	for (size_t i = 0; i < _numSprites; i++) {
		Sprite* sprite = _sprites[i];
		if (sprite->Flags & SpriteFlagManual || !(sprite->Flags & SpriteFlagVisible)) {
			continue;
		}
		dst.w = sprite->OffsetAndSizeRectF.w;
		dst.h = sprite->OffsetAndSizeRectF.h;
		SpriteDrawManual(sprite, &dst, &sprite->DrawColor, true);
	}
}

void SpriteSystemShutdown(void) {
	for (size_t i = 0; i < _sizeSprites; i++) {
		SpriteDestroy(_sprites[i]);
		free(_sprites[i]);
	}
	free(_sprites);
	_sprites = NULL;
	_numSprites = 0;
	_sizeSprites = 4;
	_firstSpriteHole = (size_t)-1;	// Reset on shutdown
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
