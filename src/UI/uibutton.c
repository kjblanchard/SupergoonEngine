#include <Supergoon/Input/mouse.h>
#include <Supergoon/Input/touch.h>
#include <Supergoon/UI/uibutton.h>
void UpdateUIButton(UIObject* obj) {
	UIButton* buttonData = (UIButton*)obj->Data;
	if (!buttonData) return;

	bool mouseHover = IsMouseOverlapRect(obj->Location.x, obj->Location.y, obj->Location.w, obj->Location.h);
	bool touchHover = IsTouchOverlapRect(obj->Location.x, obj->Location.y, obj->Location.w, obj->Location.h);
	bool isHovered = mouseHover || touchHover;
	if (!mouseHover && !touchHover) {
		buttonData->MouseOverLastFrame = false;
		buttonData->TouchOverLastFrame = false;	 // reset touch tracking as well
		return;
	}

	// Hover effect
	if (buttonData->ButtonHoverEvent) {
		bool justHovered = !buttonData->MouseOverLastFrame && isHovered;
		buttonData->ButtonHoverEvent(obj, justHovered);
	}

	if (buttonData->ButtonClickEvent && IsMouseButtonJustPressed(MouseButtonsLeftClick)) {
		buttonData->ButtonClickEvent(obj);
	}

	if ((IsTouchJustPressed() || IsTouchHeldDown()) && touchHover) {
		buttonData->TouchOverLastFrame = true;
	} else if (IsTouchJustReleased()) {
		if (buttonData->TouchOverLastFrame && touchHover && buttonData->ButtonClickEvent) {
			buttonData->ButtonClickEvent(obj);
		}
		if (buttonData->TouchOverLastFrame && buttonData->ButtonClickEvent) {
			buttonData->ButtonClickEvent(obj);
		}
		buttonData->TouchOverLastFrame = false;	 // reset
	}

	buttonData->MouseOverLastFrame = isHovered;
}
