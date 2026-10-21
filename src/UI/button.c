#include <Supergoon/Input/mouse.h>
#include <Supergoon/Input/touch.h>
#include <Supergoon/UI/button.h>
#include <stdlib.h>

static void UIButtonUpdate(UIObject* o) {
	UIButtonData* data = (UIButtonData*)o->TypeData;
	if (!data) {
		return;
	}
	if (!data->ButtonActive) {
		data->CurrentMouseStatus = UIButtonMouseStatusNone;
		return;
	}
	int over = IsMouseOverlapRect(o->AbsolutePos.X, o->AbsolutePos.Y, o->Rect.w, o->Rect.h) ||
			   IsTouchOverlapRect(o->AbsolutePos.X, o->AbsolutePos.Y, o->Rect.w, o->Rect.h);
	int wasOver = data->CurrentMouseStatus == UIButtonMouseStatusJustOverlap ||
				  data->CurrentMouseStatus == UIButtonMouseStatusOverlap;
	if (over) {
		data->CurrentMouseStatus = wasOver ? UIButtonMouseStatusOverlap : UIButtonMouseStatusJustOverlap;
	} else {
		data->CurrentMouseStatus = wasOver ? UIButtonMouseStatusJustLeftOverlap : UIButtonMouseStatusNone;
	}
}

// TypeData is freed by UIObjectDestroy, so no destroy func
static UIType uiButtonType = {"Button", NULL, NULL, NULL, UIButtonUpdate};

UIObject* CreateUIButton(UIButtonArgs* args) {
	UIButtonData* data = malloc(sizeof(UIButtonData));
	data->ButtonActive = 1;
	data->CurrentMouseStatus = UIButtonMouseStatusNone;
	args->Object->Type = &uiButtonType;
	args->Object->TypeData = data;
	return args->Object;
}

void UIButtonSetActive(UIObject* o, int active) {
	UIButtonData* data = (UIButtonData*)o->TypeData;
	if (!data || o->Type != &uiButtonType) {
		return;
	}
	data->ButtonActive = active ? 1 : 0;
	if (!active) {
		data->CurrentMouseStatus = UIButtonMouseStatusNone;
	}
}

UIButtonMouseStatus UIButtonGetMouseStatus(UIObject* o) {
	UIButtonData* data = (UIButtonData*)o->TypeData;
	if (!data || o->Type != &uiButtonType) {
		return UIButtonMouseStatusNone;
	}
	return data->CurrentMouseStatus;
}
