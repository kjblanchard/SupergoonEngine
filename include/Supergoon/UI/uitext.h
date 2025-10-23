#pragma once
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/UI/uiobject.h>
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
} UIText;

void InitializeUITextSystem(void);
void ShutdownUITextSystem(void);

void SetTextColor(UIObject* uiobject, int r, int g, int b, int a);
void SetTextSize(UIObject* uiobject, int size);
void MeasureText(UIObject* uiobject);
void SetFont(const char* fontName, unsigned int size);
void SetCenteredX(UIObject* uiobject, int centered);
void SetCenteredY(UIObject* uiobject, int centered);
void UITextLoad(UIObject* object);
void UITextOnDirty(UIObject* object);
void UITextDraw(UIObject* object);
void DestroyUIText(UIObject* object);

#ifdef __cplusplus
}
#endif
