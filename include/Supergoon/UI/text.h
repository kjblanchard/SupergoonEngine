
#pragma once
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/UI/object.h>
#ifdef __cplusplus
extern "C" {
#endif

struct Text;

typedef struct UITextData {
	struct Text* Text;
	Color Color;
	int NumDrawLetters;
} UITextData;

typedef struct UITextArgs {
	UIObject* Object;
	struct Text* Text;
	Color Color;
} UITextArgs;

UIObject* CreateUIText(UITextArgs* args);
void UITextUpdateNumDrawLetters(UIObject* o, int n);

#ifdef __cplusplus
}
#endif
