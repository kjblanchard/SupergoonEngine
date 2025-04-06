#pragma once
#include <Supergoon/UI/uiobject.h>
#include <Supergoon/graphics.h>
#include <ft2build.h>
// #include <ft2build.h>
#include FT_FREETYPE_H
#ifdef __cplusplus
extern "C" {
#endif
struct LoadedFont;

typedef struct UIText {
	//  Text to display
	char* Text;
	struct LoadedFont* Font;
	//  Size of the font
	unsigned int FontSize;
	//  Should we word wrap?
	int WordWrap;
	//  To know where we are at when drawing
	int PenX, PenY;
	//  For drawing to the Texture
	unsigned int NumLettersToDraw, CurrentDrawnLetters;
	//  For alignment purposes, set when measuring text pass so we know where to set the penx/y on newlines.
	// unsigned int TextStartX, TextStartY;
	// For word wrapping, so we know what characters we should word wrap with
	unsigned int NumWordWrapCharacters;
	// For word wrapping, so we know what characters we should word wrap with
	unsigned int* WordWrapCharacters;
	// Texture with the rendered text to be drawn
	Texture* Texture;
} UIText;

void InitializeUITextSystem();

void MeasureText(UIObject* uiobject);
void SetFont(const char* fontName, unsigned int size);
void UITextLoad(UIObject* object);
void UITextOnDirty(UIObject* object);
void UITextDraw(UIObject* object);

#ifdef __cplusplus
}
#endif
