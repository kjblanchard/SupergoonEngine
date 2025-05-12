#pragma once
#include <Supergoon/UI/uiobject.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UIButton {
	int MouseOverLastFrame;
	void (*ButtonClickEvent)(UIObject* obj);
	void (*ButtonHoverEvent)(UIObject* obj, int justHovered);
} UIButton;

void UpdateUIButton(UIObject* obj);
#ifdef __cplusplus
}
#endif
