#pragma once
#include <Supergoon/UI/uiobject.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UIButton {
	int MouseOverLastFrame;
	int TouchOverLastFrame;
	int ClickedLastFrame;
	// Used on touch, and if you should click on release.
	int ClickOnRelease;
	void (*ButtonClickEvent)(UIObject* obj, int justClicked);
	void (*ButtonHoverEvent)(UIObject* obj, int justHovered);
} UIButton;

void UpdateUIButton(UIObject* obj);
#ifdef __cplusplus
}
#endif
