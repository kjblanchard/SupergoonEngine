#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/camera.h>
#include <Supergoon/sprite.h>
#include <sgtools/tools.h>
#include <string.h>

static size_t countSprites = 0;
static size_t sizeSprites = 0;
static Sprite** sprites;

Sprite* SpriteNew(void) {
	ArrayResizeIfNeeded((void**)&sprites, countSprites + 1, &sizeSprites, sizeof(Sprite*));
	Sprite* sprite = calloc(1, sizeof(*sprite));
	sprite->Visible = true;
	sprite->Scale = 1.0f;
	sprite->DrawColor = (Color){255, 255, 255, 255};
	sprites[countSprites++] = sprite;
	return sprite;
}

void SpriteDestroy(Sprite* sprite) {
	int find = -1;
	for (int i = 0; i < countSprites; ++i) {
		if (sprites[i] == sprite) {
			find = i;
		}
	}
	if (find >= 0) {
		ArrayRemoveFromUnsorted((void*)sprites, &countSprites, sizeSprites, (size_t)find);
	}
	sgLogDebug("Destroying sprite %p", sprite);
	free(sprite);
}

void SpriteDrawManual(Sprite* sprite, Color* color, int camera) {
	if (!sprite || !sprite->Texture || !sprite->Visible) {
		return;
	}
	RectangleF dst = sprite->Location;
	if (camera) {
		float alpha = CameraGetInterpolationAlpha();
		dst.x = sprite->PreviousX + alpha * (sprite->Location.x - sprite->PreviousX);
		dst.y = sprite->PreviousY + alpha * (sprite->Location.y - sprite->PreviousY);
	}
	DrawTexture(sprite->Texture, sprite->Shader, &dst, &sprite->TextureSourceRect, camera, sprite->Scale, false, color);
}

// void SpriteDrawManual(Sprite* sprite, Color* color, int camera) {
// 	if (!sprite || !sprite->Texture || !sprite->Visible) {
// 		return;
// 	}
// 	RectangleF dst = sprite->Location;
// 	if (camera) {
// 		float interpX = sprite->PreviousX + CameraGetInterpolationAlpha() * (sprite->Location.x - sprite->PreviousX);
// 		dst.x = interpX + sprite->Location.x;
// 		float interpY = sprite->PreviousY + CameraGetInterpolationAlpha() * (sprite->Location.y - sprite->PreviousY);
// 		dst.y = interpY + sprite->Location.y;
// 	}
// DrawTexture(sprite->Texture, sprite->Shader, &dst, &sprite->TextureSourceRect, camera, sprite->Scale, false, color);
// }

void SpriteSystemUpdate(void) {
	for (size_t i = 0; i < countSprites; i++) {
		Sprite* sprite = sprites[i];
		sprite->PreviousX = sprite->Location.x;
		sprite->PreviousY = sprite->Location.y;
	}
}

void SpriteSystemDraw(void) {
	for (size_t i = 0; i < countSprites; i++) {
		Sprite* sprite = sprites[i];
		if (sprite->Manual || !sprite->Visible) {
			continue;
		}
		SpriteDrawManual(sprite, &sprite->DrawColor, true);
	}
}

void SpriteSystemShutdown(void) {
	free((void*)sprites);
	sizeSprites = 0;
	countSprites = 0;
}
