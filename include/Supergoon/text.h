#pragma once
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

struct Directory;

// Engine func
void InitializeTextSystem(void);
// Engine func
void ShutdownTextSystem(void);
// Loads a font to be used, caches fonts internally based on name/size
int TextSetFont(const char* fontName, unsigned int size, struct Directory* directory);

/**
 * @brief Text object, used for text that will not change much, or if you need to type some characters out.  Cretes a render target and writes the characters to it.  Also handles centering, etc.  Drawing is simple
 */
typedef struct Text {
	RectangleF Location;
	char* Text;
	struct LoadedFont* Font;
	unsigned int TextSizeX;
	unsigned int TextSizeY;
	int PenX, PenY;
	unsigned int NumLettersToDraw, CurrentDrawnLetters;
	int TextStartX, TextStartY;
	unsigned int NumWordWrapCharacters;
	unsigned int* WordWrapCharacters;
	struct Color Color;
	Texture* Texture;
	unsigned int WordWrap : 1;
	unsigned int CenteredX : 1;
	unsigned int CenteredY : 1;
} Text;

Text* TextCreate(RectangleF* location, const char* text);
void TextLoad(Text* text);
void TextOnDirty(Text* text);
// Used if we need to redraw all of the text, usually done if recentering, resizing, etc
void TextRedrawText(Text* text);
void TextDraw(Text* text, Color* color);
void TextDestroy(Text* text);
/**
 * @brief Draws a string without caching, word wrap, etc
 *
 * @param str The string to draw
 * @param fontName name of the font
 * @param size font size
 * @param x location to draw
 * @param y location to draw
 * @param color color of the text
 * @param useCamera camera offset
 *
 * @return width of pixels drawn
 */
int TextDrawStringDirect(const char* str, const char* fontName, unsigned int size, float x, float y, Color* color, int useCamera);

/**
 * @brief Gets the width of what we would draw
 *
 * @param str text to draw
 * @param fontName font name
 * @param size font size
 *
 * @return width in pixels to draw
 */
int TextMeasureStringDirect(const char* str, const char* fontName, unsigned int size);

#ifdef __cplusplus
}
#endif
