#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/sprite.h>
#include <Supergoon/text.h>
#include <assert.h>
#include <ft2build.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include FT_FREETYPE_H

#define MAX_LOADED_FONTS 12
#define ASCII_CHAR_NUM 128
static FT_Library _loadedLibrary = NULL;
typedef struct LoadedFont {
	char* FontName;
	FT_Face FontFace;
	int FontSize;
	Texture* GlyphTextures[ASCII_CHAR_NUM];
} LoadedFont;
static LoadedFont _loadedFonts[MAX_LOADED_FONTS];
static LoadedFont* _currentFont = NULL;

static void loadTexturesForFont(LoadedFont* font) {
	FT_Set_Pixel_Sizes(font->FontFace, 0, font->FontSize);
	for (size_t i = 0; i < ASCII_CHAR_NUM; i++) {
		if (FT_Load_Char(font->FontFace, i, FT_LOAD_RENDER)) {
			sgLogWarn("Freetype failed to load glyph!");
			continue;
		}
		Texture* texture = TextureCreateNoCache();
		font->GlyphTextures[i] = texture;
		char name[2] = {i, '\0'};
		TextureLoadFromData(texture, name, font->FontFace->glyph->bitmap.width, font->FontFace->glyph->bitmap.rows, font->FontFace->glyph->bitmap.buffer);
	}
}

static int getLetterYOffset(char letter, LoadedFont* font) {
	int error = FT_Load_Char(font->FontFace, letter, FT_LOAD_DEFAULT);
	if (error) {
		sgLogError("Could not measure character properly.  Char %s, error %d", letter, error);
		return 0;
	}
	return font->FontFace->glyph->bitmap_top;
}

static int getLetterXOffset(Text* text, int i) {
	int error = FT_Load_Char(text->Font->FontFace, text->Text[i], FT_LOAD_DEFAULT);
	if (error) {
		sgLogError("Could not measure character properly.  Char %s, error %d", text->Text[i], error);
		return 0;
	}
	return text->Font->FontFace->glyph->bitmap_left;
}

static int getKerning(int i, char* text, LoadedFont* font) {
	if (strlen(text) <= (size_t)i + 1) {
		return 0;
	}
	if (!FT_HAS_KERNING(font->FontFace)) {
		return 0;
	}
	unsigned int glyph_index_c = FT_Get_Char_Index(font->FontFace, text[i]);
	unsigned int glyph_index_n = FT_Get_Char_Index(font->FontFace, text[i + 1]);
	FT_Vector delta;
	int result = FT_Get_Kerning(font->FontFace, glyph_index_c, glyph_index_n, FT_KERNING_DEFAULT, &delta);
	if (result) {
		sgLogError("Could not get kerning");
	}
	result = delta.x >> 6;
	if (delta.x != 0) {
		sgLogWarn("Wow theres actually kerning and this hasen't been tested");
	}
	return result;
}

static int getLetterAdvance(Text* text, int i) {
	if (text->Text[i] == '\n' || text->Text[i] == '\r') {
		return 0;
	}
	int result = FT_Load_Char(text->Font->FontFace, text->Text[i], FT_LOAD_DEFAULT);
	if (result) {
		sgLogError("Could not measure character properly.  Char %s, error %d", text->Text[i], result);
		return 0;
	}
	return text->Font->FontFace->glyph->advance.x >> 6;
}

static bool shouldWrap(int x, int wordLength, int glyphWidth, int maxX) {
	return x + wordLength + glyphWidth > maxX;
}

static int getCenteredXPenLoc(Text* text) {
	int x = (text->Location.w - text->TextSizeX) / 2;
	return x < 0 ? 0 : x;
}

static int getCenteredYPenLoc(Text* text) {
	int yTop = (text->Font->FontFace->ascender * text->Font->FontSize) / text->Font->FontFace->units_per_EM;
	int y = (text->Location.h - text->TextSizeY) / 2;
	y = yTop + y;
	return yTop < y ? y : yTop;
}

static void drawLetter(Text* text) {
	// Check to see if we should move to the next line:
	for (size_t i = 0; i < text->NumWordWrapCharacters; i++) {
		if (text->WordWrapCharacters[i] == text->CurrentDrawnLetters) {
			text->PenX = 0;
			if (text->CenteredX) {
				text->PenX = getCenteredXPenLoc(text);
			}
			int ascenderInPixels = (text->Font->FontFace->ascender * text->Font->FontSize) / text->Font->FontFace->units_per_EM;
			int descenderInPixels = (text->Font->FontFace->descender * text->Font->FontSize) / text->Font->FontFace->units_per_EM;
			text->PenY += ascenderInPixels - descenderInPixels + 1;
			break;
		}
	}
	Texture* texture = text->Font->GlyphTextures[(unsigned char)text->Text[text->CurrentDrawnLetters]];
	// Draw text If text is a space, and we are at beginning of a line, do not draw it }
	if (text->Text[text->CurrentDrawnLetters] == ' ' && text->WordWrap && text->PenX == text->TextStartX) {
		return;
	}
	int x = text->PenX + getLetterXOffset(text, text->CurrentDrawnLetters) - getKerning(text->CurrentDrawnLetters, text->Text, text->Font);
	int y = text->PenY - getLetterYOffset(text->Text[text->CurrentDrawnLetters], text->Font);
	float w = TextureGetWidth(texture);
	float h = TextureGetHeight(texture);
	RectangleF dst = {x, y, w, h};
	RectangleF src = {0, 0, w, h};
	if (!text->Texture) {
		sgLogError("How is there no texture here:??");
		return;
	}
	//  If this is a drawable texture, draw it and advance
	if (texture) {
		SetRenderTarget(text->Texture);
		Color color = {255, 0, 0, 255};
		DrawTexture(texture, GetDefaultTextShader(), &dst, &src, false, 1.0, false, &color);
		SetPreviousRenderTarget();
	}
	text->PenX += getLetterAdvance(text, text->CurrentDrawnLetters);
}

static LoadedFont* getLoadedFont(const char* fontName, unsigned int size) {
	char fontAndSize[255];
	snprintf(fontAndSize, sizeof(fontAndSize), "%s%d", fontName, size);
	for (size_t i = 0; i < MAX_LOADED_FONTS; i++) {
		if (_loadedFonts[i].FontName && strcmp(fontAndSize, _loadedFonts[i].FontName) == 0) {
			return &_loadedFonts[i];
		}
	}
	LoadedFont* fontToLoadInto = NULL;
	for (size_t i = 0; i < MAX_LOADED_FONTS; i++) {
		if (!_loadedFonts[i].FontName) {
			fontToLoadInto = &_loadedFonts[i];
			break;
		}
	}
	if (!fontToLoadInto) {
		sgLogError("Could not load font, you have used the max fonts/sizes, pleas up MAX_LOADED_FONTS in uitext.c");
		return NULL;
	}
	fontToLoadInto->FontName = strdup(fontAndSize);
	snprintf(fontAndSize, sizeof(fontAndSize), "assets/fonts/%s.ttf", fontName);
	char fullFilepath[1000];
	GetFilenameWithExeFilepath(fullFilepath, sizeof(fullFilepath), fontAndSize);
	int result = FT_New_Face(_loadedLibrary, fullFilepath, 0, &fontToLoadInto->FontFace);
	// TODO probably should cleanup here even though it may never happen
	if (result) {
		sgLogError("Could not open font %s with error %d\n", fullFilepath, result);
		return NULL;
	}
	fontToLoadInto->FontSize = size;
	loadTexturesForFont(fontToLoadInto);
	return fontToLoadInto;
}

static void addWordToWrapPoints(unsigned int currentWordWraps, Text* text, int location) {
	if (currentWordWraps + 1 > text->NumWordWrapCharacters) {
		unsigned int* newArray = realloc(text->WordWrapCharacters, sizeof(int) * (currentWordWraps + 1));
		if (!newArray) {
			sgLogCritical("Could not allocate properly when creating text, now what");
		}
		text->WordWrapCharacters = newArray;
	}
	text->WordWrapCharacters[currentWordWraps] = location;
	++text->NumWordWrapCharacters;
}

static void measureText(Text* text) {
	FT_Face fontFace = text->Font->FontFace;
	assert(fontFace && text && "no font loaded for text to load");
	// Max width and height will be the size of the uiobject, this should be loaded prior
	int currentWordLength = 0;
	int currentWordLetters = 0;
	int ascenderInPixels = (fontFace->ascender * text->Font->FontSize) / fontFace->units_per_EM;
	int descenderInPixels = (fontFace->descender * text->Font->FontSize) / fontFace->units_per_EM;
	int lineSpace = (fontFace->height * text->Font->FontSize) / fontFace->units_per_EM;
	int startLoc = ascenderInPixels;
	// Used to determine when to start for alignments, after we measure the text.
	int textSizeX = 0, textSizeY = 0;
	int penX = 0;
	int penY = startLoc;
	int currentWordWraps = 0;
	for (size_t i = 0; i < strlen(text->Text); i++) {
		char letter = text->Text[i];
		// If this is a newline, we should add to our letter points if there is a word
		if (letter == '\n' || letter == '\r') {
			if (currentWordLength) {
				addWordToWrapPoints(currentWordWraps, text, i);
				penX += currentWordLength;
				if (penX > textSizeX) {
					textSizeX = penX;
				}
			}
			penX = 0;
			penY += lineSpace;
			++currentWordWraps;
			currentWordLetters = 0;
			currentWordLength = 0;
			continue;
		}
		int letterSize = getLetterAdvance(text, i);
		if (letter == ' ') {
			penX += currentWordLength;
			penX += letterSize;
			currentWordLetters = 0;
			currentWordLength = 0;
			// We probably should see if we are wrapping now.  If we wrap, then we shouldn't count this.
			continue;
		} else {
			currentWordLength += letterSize;
		}
		// If we should wrap to the next line, move penx to beginning, and increment peny
		if (shouldWrap(penX, currentWordLength, 0, text->Location.w)) {
			addWordToWrapPoints(currentWordWraps, text, i - currentWordLetters);
			// If current pen location is greater than the calculated text size, update
			if (penX > textSizeX) {
				textSizeX = penX;
			}
			penX = currentWordLength;
			penY += lineSpace;
			++currentWordWraps;
			currentWordLength = 0;
			currentWordLetters = 0;
		} else {
			++currentWordLetters;
		}
		// If we shouldn't word wrap, treat every letter like it's own word.
		if (!text->WordWrap) {
			penX += letterSize;
			currentWordLength = 0;
			currentWordLetters = 0;
		}
	}
	// Once the word is completed, we should cleanup
	if (currentWordLength) {
		penX += currentWordLength;
	}
	textSizeX = textSizeX > penX ? textSizeX : penX;
	textSizeY = penY - descenderInPixels;
	text->TextSizeX = textSizeX;
	text->TextSizeY = textSizeY;
	if (textSizeY > text->Location.h) {
		sgLogWarn("Your text overflowed through Y, please adjust your bounds else it will flow past");
	}
	if (text->CenteredX) {
		text->TextStartX = getCenteredXPenLoc(text);
	}
}

// Used if we need to redraw all of the text, usually done if recentering, resizing, etc
void TextRedrawText(Text* text) {
	if (text->Texture) {
		TextureDestroy(text->Texture);
	}
	text->Texture = TextureCreateRenderTarget(text->Location.w, text->Location.h);
	text->PenX = 0;
	text->CurrentDrawnLetters = 0;
	text->NumWordWrapCharacters = 0;
	measureText(text);
	if (text->CenteredX) {
		text->PenX = getCenteredXPenLoc(text);
	}
	if (text->CenteredY) {
		text->PenY = getCenteredYPenLoc(text);

	} else {
		text->PenY = (text->Font->FontFace->ascender * text->Font->FontSize) / text->Font->FontFace->units_per_EM;
	}
}

void InitializeTextSystem(void) {
	if (!_loadedLibrary) {
		if (FT_Init_FreeType(&_loadedLibrary)) {
			sgLogWarn("Could not initialize FreeType library");
		}
	}
}
void ShutdownTextSystem(void) {
	for (size_t i = 0; i < MAX_LOADED_FONTS; i++) {
		LoadedFont* font = &_loadedFonts[i];
		if (!font) {
			break;
		}
		free(font->FontName);
		for (size_t j = 0; j < ASCII_CHAR_NUM; j++) {
			TextureDestroy(font->GlyphTextures[j]);
		}
		FT_Done_Face(font->FontFace);
	}
	if (_loadedLibrary) FT_Done_FreeType(_loadedLibrary);
}

int TextSetFont(const char* fontName, unsigned int size) {
	if (!fontName) {
		sgLogWarn("No font name, not setting");
		return false;
	}
	if (size > 999 || size < 1) {
		sgLogWarn("Improper size passed into font, must be between 1 and 1000, setting to 32.");
		size = 32;
	}
	_currentFont = getLoadedFont(fontName, size);
	return _currentFont != NULL;
}

void TextLoad(Text* text) {
	TextOnDirty(text);
}

void TextOnDirty(Text* text) {
	if (text && !text->Texture) {
		TextRedrawText(text);
	}
	// Recreate texture if the size has changed
	float w = TextureGetWidth(text->Texture);
	float h = TextureGetHeight(text->Texture);
	if (text->Location.h != h || text->Location.w != w) {
		TextRedrawText(text);
	}
	// If there is more letters drawn than the current amount to draw, clear the texture and start from 0
	if (text->NumLettersToDraw < text->CurrentDrawnLetters) {
		TextureClearRenderTarget(text->Texture, 0, 0, 0, 0);
		text->CurrentDrawnLetters = 0;
		text->PenX = getCenteredXPenLoc(text);
		text->PenY = getCenteredYPenLoc(text);
	}
	unsigned int lettersToWrite = text->NumLettersToDraw > strlen(text->Text) ? strlen(text->Text) : text->NumLettersToDraw;
	while (text->CurrentDrawnLetters < lettersToWrite) {
		drawLetter(text);
		++text->CurrentDrawnLetters;
	}
}

Text* TextCreate(RectangleF* location, const char* textText) {
	Text* text = calloc(1, sizeof(*text));
	text->Text = strdup(textText);
	text->NumLettersToDraw = strlen(textText);
	text->Location = *location;
	text->Font = _currentFont;
	return text;
}

void TextDraw(Text* text, float parentX, float parentY) {
	if (!text->Texture) return;
	RectangleF src = {0, 0, text->Location.w, text->Location.h};
	RectangleF dst = {text->Location.x + parentX, text->Location.y + parentY, text->Location.w, text->Location.h};
	Color color = {255, 255, 255, 255};
	DrawTexture(text->Texture, GetDefaultShader(), &dst, &src, false, 1.0, false, &color);
}

void TextDestroy(Text* text) {
	sgLogWarn("Destroying the text!");
	TextureDestroy(text->Texture);
	free(text->Text);
	text->Texture = NULL;
	text->Text = NULL;
	free(text);
}
