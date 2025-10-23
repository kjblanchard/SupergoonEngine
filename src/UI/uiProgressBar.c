#include <Supergoon/UI/uiProgressBar.h>
// #include <Supergoon/graphics.h>

void DrawProgressBar(UIObject* uiobject) {
	if (!(uiobject && uiobject->Data && uiobject->Type == UIObjectTypesProgressBar)) {
		return;
	}
	UIProgressBarData* data = (UIProgressBarData*)uiobject->Data;
	if (!data) {
		return;
	}
	RectangleF dst = {uiobject->Location.x,
					  uiobject->Location.y,
					  uiobject->Location.w * data->BarPercent / 100,
					  uiobject->Location.h};
	// DrawRect(&dst, &data->Color, true);
}
