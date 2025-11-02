#pragma once
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/UI/uiobject.h>
#ifdef __cplusplus
extern "C" {
#endif
struct LoadedFont;

typedef struct Text {
	char* Text;
	struct LoadedFont* Font;
	int WordWrap;
	unsigned int TextSizeX;
	unsigned int TextSizeY;
	int CenteredX;
	int CenteredY;
	//  To know where we are at when drawing
	int PenX, PenY;
	//  For drawing to the Texture
	unsigned int NumLettersToDraw, CurrentDrawnLetters;
	//  For alignment purposes, set when measuring text pass so we know where to set the penx/y on newlines.
	int TextStartX, TextStartY;
	// For word wrapping, so we know what characters we should word wrap with
	unsigned int NumWordWrapCharacters;
	// For word wrapping, so we know what characters we should word wrap with
	unsigned int* WordWrapCharacters;
	sgColor Color;
	// Texture with the rendered text to be drawn
	Texture* Texture;
} Text;

void InitializeTextSystem(void);
void ShutdownTextSystem(void);
Text* TextCreate(void);
void SetTextColor(Text* text, int r, int g, int b, int a);
void SetTextSize(Text* text, int size);
void MeasureText(Text* text);
int TextSetFont(const char* fontName, unsigned int size);
void SetCenteredX(Text* text, int centered);
void SetCenteredY(Text* text, int centered);
void TextLoad(Text* text);
void UITextOnDirty(Text* text);
void TextDraw(Text* text);
void DestroyUIText(Text* text);

#ifdef __cplusplus
}
#endif
