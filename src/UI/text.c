#include <Supergoon/UI/text.h>
#include <Supergoon/sprite.h>
#include <Supergoon/text.h>
#include <assert.h>
#include <string.h>

static void UITextOnDirty(UIObject* o) {
	if (!o) return;
	UITextData* data = (UITextData*)o->TypeData;
	assert(data && "No data for text!");
	TextOnDirty(data->Text);
}

static void UITextDraw(UIObject* o) {
	if (!o) return;
	UITextData* data = (UITextData*)o->TypeData;
	assert(data && "No data for text!");
	data->Text->Location.x = o->AbsolutePos.X;
	data->Text->Location.y = o->AbsolutePos.Y;
	TextDraw(data->Text, &data->Color);
}

static void UITextDestroy(UIObject* o) {
	if (!o) return;
	UITextData* data = (UITextData*)o->TypeData;
	assert(data && "No data for text!");
	TextDestroy(data->Text);
}

static UIType uiTextType = {"Text", UITextOnDirty, UITextDraw, UITextDestroy};

UIObject* CreateUIText(UITextArgs* args) {
	UITextData* data = malloc(sizeof(UITextData));
	data->Color = args->Color;
	args->Object->Type = &uiTextType;
	data->Text = args->Text;
	data->NumDrawLetters = (int)strlen(data->Text->Text);
	args->Object->TypeData = data;
	args->Object->Dirty = true;
	return args->Object;
}

void UITextUpdateNumDrawLetters(UIObject* o, int n) {
	if (!o) return;
	UITextData* data = (UITextData*)o->TypeData;
	assert(data && "No data for text!");
	if (n == data->NumDrawLetters || n < 0 || n > strlen(data->Text->Text)) return;
	data->NumDrawLetters = n;
	data->Text->NumLettersToDraw = (unsigned int)n;
	o->Dirty = true;
}
