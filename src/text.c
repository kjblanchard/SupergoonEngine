#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/text.h>
#include <ft2build.h>
#include <stdlib.h>
#include <string.h>
#include FT_FREETYPE_H

#define MAX_LOADED_FONTS 120
#define ASCII_CHAR_NUM 128
static FT_Library _loadedLibrary = NULL;
typedef struct LoadedFont {
	char* FontName;
	FT_Face FontFace;
	// Support ASCII only
	Texture* GlyphTextures[ASCII_CHAR_NUM];
} LoadedFont;
static LoadedFont _loadedFonts[MAX_LOADED_FONTS];
static LoadedFont* _currentFont = NULL;

static void loadTexturesForFont(LoadedFont* font, int size) {
	FT_Set_Pixel_Sizes(font->FontFace, 0, size);
	for (size_t i = 0; i < ASCII_CHAR_NUM; i++) {
		// load character glyph
		if (FT_Load_Char(font->FontFace, i, FT_LOAD_RENDER)) {
			sgLogWarn("Freetype failed to load glyph!");
			continue;
		}
		Texture* texture = TextureCreate();
		font->GlyphTextures[i] = texture;
		char name[2] = {i, '\0'};
		TextureLoadFromData(texture, name, font->FontFace->glyph->bitmap.width, font->FontFace->glyph->bitmap.rows, font->FontFace->glyph->bitmap.buffer);
	}
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
	loadTexturesForFont(fontToLoadInto, size);
	return fontToLoadInto;
}

void InitializeTextSystem(void) {
	if (!_loadedLibrary) {
		if (FT_Init_FreeType(&_loadedLibrary)) {
			sgLogWarn("Could not initialize FreeType library");
		}
	}
}
void ShutdownTextSystem(void) {
	if (_loadedLibrary) FT_Done_FreeType(_loadedLibrary);
}
void SetTextColor(Text* text, int r, int g, int b, int a) {
}
void SetTextSize(Text* text, int size) {
}
void MeasureText(Text* text) {
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
void SetCenteredX(Text* text, int centered) {
}
void SetCenteredY(Text* text, int centered) {
}

void TextLoad(Text* text) {
	text->Font = _currentFont;
}

void UITextOnDirty(Text* text) {
}

Text* TextCreate(void) {
	Text* text = calloc(1, sizeof(*text));
	return text;
}

void TextDraw(Text* text) {
	Texture* letterTexture = text->Font->GlyphTextures[65];
	int width = TextureGetWidth(letterTexture);
	int height = TextureGetHeight(letterTexture);
	RectangleF dst = {0, 0, width, height};
	RectangleF src = {0, 0, width, height};
	DrawTexture(text->Font->GlyphTextures[65], GetDefaultTextShader(), &dst, &src, false, 1.0, false);
	/* DrawTexture(text->Font->GlyphTextures[65], GetDefaultShader(), &dst, &src, false, 1.0, false); */
}

void DestroyUIText(Text* text) {
}
