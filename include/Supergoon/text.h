#pragma once
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Text {
	RectangleF Location;
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
	struct Color Color;
	// Texture with the rendered text to be drawn
	Texture* Texture;
} Text;

void InitializeTextSystem(void);
void ShutdownTextSystem(void);
Text* TextCreate(RectangleF* location, const char* text);
int TextSetFont(const char* fontName, unsigned int size);
void TextLoad(Text* text);
void TextOnDirty(Text* text);
void TextRedrawText(Text* text);
void TextDraw(Text* text, float offsetX, float offsetY);
void TextDestroy(Text* text);

#ifdef __cplusplus
}
#endif
