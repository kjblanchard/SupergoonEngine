#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/sprite.h>
#include <Supergoon/string.h>
#include <Supergoon/text.h>
#include <assert.h>
#include <ft2build.h>
#include <sgforge/unpack.h>
#include <sgtools/log.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include FT_FREETYPE_H

#define MAX_LOADED_FONTS 12
#define ASCII_CHAR_NUM 128

static FT_Library loadedLibrary = NULL;
typedef struct LoadedFont {
	char* FontName;
	FT_Face FontFace;
	int FontSize;
	int Scale;	// GraphicsGetUIScale() when loaded; other scales are evictable
	Texture* GlyphTextures[ASCII_CHAR_NUM];
} LoadedFont;
static LoadedFont loadedFonts[MAX_LOADED_FONTS];
static LoadedFont* currentFont = NULL;
static char* currentFontName = NULL;
static unsigned int currentFontBaseSize = 0;
static struct Directory* currentFontDirectory = NULL;

static void loadTexturesForFont(LoadedFont* font) {
	FT_Set_Pixel_Sizes(font->FontFace, 0, font->FontSize);
	for (size_t i = 0; i < ASCII_CHAR_NUM; i++) {
		int result = FT_Load_Char(font->FontFace, i, FT_LOAD_RENDER);
		if (result) {
			sgLogWarn("Freetype failed to load glyph, ft error %d!", result);
			continue;
		}
		String tx = StringSprintf("%s%d", font->FontFace, font->FontSize);
		Texture* texture = TextureCreate(tx.Data);
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
	int scale = GraphicsGetUIScale();
	int x = ((int)(text->Location.w * scale) - text->TextSizeX) / 2;
	return x < 0 ? 0 : x;
}

static int getCenteredYPenLoc(Text* text) {
	int scale = GraphicsGetUIScale();
	int yTop = (text->Font->FontFace->ascender * text->Font->FontSize) / text->Font->FontFace->units_per_EM;
	int y = ((int)(text->Location.h * scale) - text->TextSizeY) / 2;
	y = yTop + y;
	return yTop < y ? y : yTop;
}

static void drawLetter(Text* text) {
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
		Color color = {255, 255, 255, 255};
		DrawTexture(texture, GetDefaultTextShader(), &dst, &src, false, 1.0, false, &color);
		SetPreviousRenderTarget();
	}
	text->PenX += getLetterAdvance(text, text->CurrentDrawnLetters);
}

static void unloadFont(LoadedFont* font) {
	free(font->FontName);
	for (size_t i = 0; i < ASCII_CHAR_NUM; i++) {
		TextureDestroy(font->GlyphTextures[i]);
	}
	if (font->FontFace) FT_Done_Face(font->FontFace);
	memset(font, 0, sizeof(*font));
}

static LoadedFont* getLoadedFont(const char* fontName, unsigned int size, struct Directory* directory) {
	char fontAndSize[255];
	snprintf(fontAndSize, sizeof(fontAndSize), "%s%d", fontName, size);
	for (size_t i = 0; i < MAX_LOADED_FONTS; i++) {
		if (loadedFonts[i].FontName && strcmp(fontAndSize, loadedFonts[i].FontName) == 0) {
			return &loadedFonts[i];
		}
	}
	int scale = GraphicsGetUIScale();
	LoadedFont* fontToLoadInto = NULL;
	for (size_t i = 0; i < MAX_LOADED_FONTS; i++) {
		if (!loadedFonts[i].FontName) {
			fontToLoadInto = &loadedFonts[i];
			break;
		}
	}
	if (!fontToLoadInto) {
		// Fonts rasterized for another game scale are only reachable through
		// Text objects that re-resolve their font before use, so reuse one.
		for (size_t i = 0; i < MAX_LOADED_FONTS; i++) {
			if (loadedFonts[i].Scale != scale) {
				sgLogDebug("Evicting font %s (scale %d) for %s", loadedFonts[i].FontName, loadedFonts[i].Scale, fontAndSize);
				unloadFont(&loadedFonts[i]);
				fontToLoadInto = &loadedFonts[i];
				break;
			}
		}
	}
	if (!fontToLoadInto) {
		sgLogError("Could not load font, you have used the max fonts/sizes, pleas up MAX_LOADED_FONTS in uitext.c");
		return NULL;
	}
	snprintf(fontAndSize, sizeof(fontAndSize), "%s.ttf", fontName);
	char* buf = NULL;
	size_t sz = 0;
	GetDataFromDirectory(fontAndSize, &buf, &sz, directory);
	if (!buf || !sz) {
		sgLogCritical("Could not load font for %s", fontAndSize);
		return NULL;
	}
	FT_Error result = FT_New_Memory_Face(
		loadedLibrary,
		(const FT_Byte*)buf,
		(FT_Long)sz,
		0,
		&fontToLoadInto->FontFace);

	if (result) {
		sgLogCritical("Could not open font from memory with error %d\n", result);
		fontToLoadInto->FontFace = NULL;
		return NULL;
	}
	// Only name the slot once loaded, so a failed load can't be found by later lookups.
	snprintf(fontAndSize, sizeof(fontAndSize), "%s%d", fontName, size);
	fontToLoadInto->FontName = strdup(fontAndSize);
	fontToLoadInto->Scale = scale;
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
	assert(text->Font && "No font loaded, please load a font previously");
	FT_Face fontFace = text->Font->FontFace;
	assert(fontFace && text && "no font loaded from ft for text to load");
	int scale = GraphicsGetUIScale();
	int maxWidth = (int)(text->Location.w * scale);
	int currentWordLength = 0;
	int currentWordLetters = 0;
	int ascenderInPixels = (fontFace->ascender * text->Font->FontSize) / fontFace->units_per_EM;
	int descenderInPixels = (fontFace->descender * text->Font->FontSize) / fontFace->units_per_EM;
	int lineSpace = (fontFace->height * text->Font->FontSize) / fontFace->units_per_EM;
	int startLoc = ascenderInPixels;
	int textSizeX = 0, textSizeY = 0;
	int penX = 0;
	int penY = startLoc;
	int currentWordWraps = 0;
	for (size_t i = 0; i < strlen(text->Text); i++) {
		char letter = text->Text[i];
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
			continue;
		} else {
			currentWordLength += letterSize;
		}
		if (shouldWrap(penX, currentWordLength, 0, maxWidth)) {
			addWordToWrapPoints(currentWordWraps, text, i - currentWordLetters);
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
		if (!text->WordWrap) {
			penX += letterSize;
			currentWordLength = 0;
			currentWordLetters = 0;
		}
	}
	if (currentWordLength) {
		penX += currentWordLength;
	}
	textSizeX = textSizeX > penX ? textSizeX : penX;
	textSizeY = penY - descenderInPixels;
	text->TextSizeX = textSizeX;
	text->TextSizeY = textSizeY;
	if (textSizeY > (int)(text->Location.h * scale)) {
		sgLogWarn("Your text overflowed through Y, please adjust your bounds else it will flow past");
	}
	if (text->CenteredX) {
		text->TextStartX = getCenteredXPenLoc(text);
	}
}

void TextRedrawText(Text* text) {
	int scale = GraphicsGetUIScale();
	if (scale != text->FontScale) {
		LoadedFont* refreshed = getLoadedFont(text->FontName, text->FontBaseSize * scale, text->FontDirectory);
		if (refreshed) {
			text->Font = refreshed;
			text->FontScale = scale;
		}
		// else: cache full (MAX_LOADED_FONTS) or load failure - leave Font/FontScale
		// untouched, keep drawing with the stale-resolution font rather than
		// crashing or dropping text.
	}
	if (text->Texture) {
		TextureDestroy(text->Texture);
	}
	int physW = (int)(text->Location.w * scale);
	int physH = (int)(text->Location.h * scale);
	text->Texture = TextureCreateRenderTarget(physW, physH);
	text->TextureScale = scale;
	TextureClearRenderTarget(text->Texture, 0, 0, 0, 0);
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
	if (!loadedLibrary) {
		if (FT_Init_FreeType(&loadedLibrary)) {
			sgLogWarn("Could not initialize FreeType library");
		}
	}
}

void ShutdownTextSystem(void) {
	for (size_t i = 0; i < MAX_LOADED_FONTS; i++) {
		unloadFont(&loadedFonts[i]);
	}
	if (loadedLibrary) FT_Done_FreeType(loadedLibrary);
}

int TextSetFont(const char* fontName, unsigned int size, struct Directory* directory) {
	if (!fontName) {
		sgLogWarn("No font name, not setting");
		return false;
	}
	if (size > 999 || size < 1) {
		sgLogWarn("Improper size passed into font, must be between 1 and 1000, setting to 32.");
		size = 32;
	}
	free(currentFontName);
	currentFontName = strdup(fontName);
	currentFontBaseSize = size;
	currentFontDirectory = directory;
	unsigned int physicalSize = size * GraphicsGetUIScale();
	currentFont = getLoadedFont(fontName, physicalSize, directory);
	return currentFont != NULL;
}

void TextLoad(Text* text) {
	TextOnDirty(text);
}

void TextOnDirty(Text* text) {
	// Also rebuild when the game scale changed since the texture was rasterized,
	// so glyphs are re-rendered at the new physical size instead of stretched.
	if (text && (!text->Texture || text->TextureScale != GraphicsGetUIScale())) {
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
	GraphicsSetPremultipliedBlend(1);
	while (text->CurrentDrawnLetters < lettersToWrite) {
		drawLetter(text);
		++text->CurrentDrawnLetters;
	}
	GraphicsSetPremultipliedBlend(0);
}

Text* TextCreate(RectangleF* location, const char* textText) {
	Text* text = calloc(1, sizeof(*text));
	text->Text = strdup(textText);
	text->NumLettersToDraw = strlen(textText);
	text->Location = *location;
	text->Font = currentFont;
	text->FontName = strdup(currentFontName ? currentFontName : "");
	text->FontBaseSize = currentFontBaseSize;
	text->FontDirectory = currentFontDirectory;
	// 0 forces TextRedrawText to resolve Font by name at the current scale; the
	// scale may have changed (and currentFont been evicted) since TextSetFont.
	text->FontScale = 0;
	return text;
}

void TextDraw(Text* text, Color* color) {
	if (!text->Texture) return;
	RectangleF src = {0, 0, text->Location.w, text->Location.h};
	GraphicsSetPremultipliedBlend(1);
	DrawTexture(text->Texture, GetDefaultScreenShader(), &text->Location, &src, false, 1.0f, false, color);
	GraphicsSetPremultipliedBlend(0);
}

void TextDestroy(Text* text) {
	TextureDestroy(text->Texture);
	if (text->WordWrapCharacters) free(text->WordWrapCharacters);
	free(text->Text);
	free(text->FontName);
	text->Texture = NULL;
	text->Text = NULL;
	free(text);
}

static LoadedFont* findCachedFont(const char* fontName, unsigned int size) {
	char key[255];
	snprintf(key, sizeof(key), "%s%d", fontName, size);
	for (size_t i = 0; i < MAX_LOADED_FONTS; i++) {
		if (loadedFonts[i].FontName && strcmp(key, loadedFonts[i].FontName) == 0)
			return &loadedFonts[i];
	}
	// The game scale may have changed since TextSetFont loaded this font, so load
	// it at the new physical size rather than drawing nothing.
	if (currentFontDirectory) {
		return getLoadedFont(fontName, size, currentFontDirectory);
	}
	return NULL;
}

int TextMeasureStringDirect(const char* str, const char* fontName, unsigned int size) {
	int scale = GraphicsGetUIScale();
	LoadedFont* font = findCachedFont(fontName, size * scale);
	if (!font) {
		return 0;
	}
	int penX = 0;
	for (size_t i = 0; str[i] != '\0'; i++) {
		unsigned char c = (unsigned char)str[i];
		if (c >= ASCII_CHAR_NUM) continue;
		if (FT_Load_Char(font->FontFace, c, FT_LOAD_DEFAULT)) continue;
		penX += font->FontFace->glyph->advance.x >> 6;
	}
	return penX / scale;
}

int TextDrawStringDirect(const char* str, const char* fontName, unsigned int size, float x, float y, Color* color, int useCamera) {
	int scale = GraphicsGetUIScale();
	LoadedFont* font = findCachedFont(fontName, size * scale);
	if (!font) {
		return 0;
	}
	float invScale = 1.0f / scale;
	int ascender = (font->FontFace->ascender * font->FontSize) / font->FontFace->units_per_EM;
	float penX = x;
	float baseY = y + ascender * invScale;
	GraphicsSetPremultipliedBlend(1);
	for (size_t i = 0; str[i] != '\0'; i++) {
		unsigned char c = (unsigned char)str[i];
		if (c >= ASCII_CHAR_NUM) continue;
		if (FT_Load_Char(font->FontFace, c, FT_LOAD_DEFAULT)) continue;
		Texture* glyph = font->GlyphTextures[c];
		if (glyph) {
			float w = TextureGetWidth(glyph) * invScale;
			float h = TextureGetHeight(glyph) * invScale;
			float glyphX = penX + font->FontFace->glyph->bitmap_left * invScale;
			float glyphY = baseY - font->FontFace->glyph->bitmap_top * invScale;
			RectangleF dst = {glyphX, glyphY, w, h};
			RectangleF src = {0, 0, (float)TextureGetWidth(glyph), (float)TextureGetHeight(glyph)};
			DrawTexture(glyph, GetDefaultTextShader(), &dst, &src, useCamera, 1.0f, false, color);
		}
		penX += (font->FontFace->glyph->advance.x >> 6) * invScale;
	}
	GraphicsSetPremultipliedBlend(0);
	return (int)(penX - x);
}
