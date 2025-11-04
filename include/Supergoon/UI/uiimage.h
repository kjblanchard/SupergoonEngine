#pragma once
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/UI/uiobject.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UIImageData {
	Texture* Texture;
	RectangleF SrcRect;

} UIImageData;

void DrawUIImage(UIObject* uiobject);
void DestroyUIImage(UIObject* uiobject);
#ifdef __cplusplus
}
#endif
