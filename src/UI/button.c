#include <Supergoon/UI/text.h>
#include <Supergoon/sprite.h>
#include <Supergoon/text.h>
#include <string.h>

static void UITextOnDirty(UIObject* o) {
	UITextData* data = (UITextData*)o->TypeData;
	if (!data) {
		return;
	}
	TextRedrawText(data->Text);
}

static void UITextDraw(UIObject* o) {
	UITextData* data = (UITextData*)o->TypeData;
	if (!data) {
		return;
	}
	data->Text->Location.x = o->AbsolutePos.X;
	data->Text->Location.y = o->AbsolutePos.Y;
	TextDraw(data->Text, &data->Color);
}

static void UITextDestroy(UIObject* o) {
	UITextData* data = (UITextData*)o->TypeData;
	if (!data) {
		return;
	}
	TextDestroy(data->Text);
}

static UIType uiTextType = {"Text", UITextOnDirty, UITextDraw, UITextDestroy};

UIObject* CreateUIText(UITextArgs* args) {
	UITextData* data = malloc(sizeof(UITextData));
	data->Color = args->Color;
	args->Object->Type = &uiTextType;
	data->Text = args->Text;
	args->Object->TypeData = data;
	return args->Object;
}
