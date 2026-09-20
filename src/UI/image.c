#include <Supergoon/UI/image.h>
#include <Supergoon/sprite.h>
#include <string.h>

static void UIImageDraw(UIObject* o) {
	UIImageData* data = (UIImageData*)o->TypeData;
	if (!data) {
		return;
	}
	data->Sprite->ParentX = o->Parent ? o->Parent->AbsolutePos.X : 0;
	data->Sprite->ParentY = o->Parent ? o->Parent->AbsolutePos.Y : 0;
	SpriteDrawManual(data->Sprite, &o->Rect, &(Color){255, 255, 255, 255}, false);
}

static void UIImageDestroy(UIObject* o) {
	UIImageData* data = (UIImageData*)o->TypeData;
	if (!data) {
		return;
	}
	SpriteDestroy(data->Sprite);
	free(o->TypeData);
}

static UIType uiImageType = {"Image", NULL, UIImageDraw, UIImageDestroy};

UIObject* CreateUIImage(UIImageArgs* args) {
	UIImageData* data = malloc(sizeof(UIImageData));
	args->Object->Type = &uiImageType;
	data->Sprite = args->Sprite;
	args->Object->TypeData = data;
	return args->Object;
}
