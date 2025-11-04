#pragma once
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/UI/uiobject.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UIRect {
	Color Color;
} UIRect;

void UIRectDraw(UIObject* object);
#ifdef __cplusplus
}
#endif
