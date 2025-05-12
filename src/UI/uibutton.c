#include <Supergoon/Input/mouse.h>
#include <Supergoon/UI/uibutton.h>

void UpdateUIButton(UIObject* obj) {
	UIButton* buttonData = (UIButton*)obj->Data;
	if (!buttonData) {
		return;
	}
	if (!IsMouseOverlapRect(obj->Location.x, obj->Location.y, obj->Location.w, obj->Location.h)) {
		buttonData->MouseOverLastFrame = false;
		return;
	}
	if (buttonData->ButtonHoverEvent) {
		bool justHovered = buttonData->MouseOverLastFrame ? false : true;
		buttonData->ButtonHoverEvent(obj, justHovered);
	}
	if (buttonData->ButtonClickEvent || IsMouseButtonJustPressed(MouseButtonsLeftClick)) {
		buttonData->ButtonClickEvent(obj);
	}
	buttonData->MouseOverLastFrame = true;
}
