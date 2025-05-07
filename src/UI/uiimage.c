#include <Supergoon/UI/uiimage.h>

void DrawUIImage(UIObject* uiobject) {
	UIImageData* data = (UIImageData*)uiobject->Data;
	if (!data) {
		return;
	}
	RectangleF* src = (data->SrcRect.x == 0 && data->SrcRect.y == 0 && data->SrcRect.w == 0 && data->SrcRect.h == 0) ? NULL : &data->SrcRect;
	DrawTexture(data->Texture, &uiobject->Location, src);
}

void DestroyUIImage(UIObject* uiobject) {
	UIImageData* data = (UIImageData*)uiobject->Data;
	if (!data) {
		return;
	}
	UnloadTexture(data->Texture);
}
