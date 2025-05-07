#include <SDL3/SDL.h>
#include <Supergoon/UI/uiobject.h>
#include <Supergoon/UI/uitext.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/graphics.h>
#include <SupergoonEngine/tools.h>
#include <assert.h>
#include <ft2build.h>
#include <math.h>
#include FT_FREETYPE_H
#define MAX_LOADED_FONTS 12
#define ASCII_CHAR_NUM 127
static FT_Library _loadedLibrary = NULL;
// TODO we should cache fonts (faces) when we load them, so that we can use multiple.
typedef struct LoadedFont {
	char* FontName;
	FT_Face FontFace;
	// Support ASCII only
	Texture* GlyphTextures[ASCII_CHAR_NUM];
} LoadedFont;

static LoadedFont _loadedFonts[MAX_LOADED_FONTS];
static LoadedFont* _currentFont = NULL;

void SetTextColor(UIObject* uiobject, int r, int g, int b, int a) {
	if (!uiobject) {
		return;
	}
	UIText* text = (UIText*)uiobject->Data;
	assert(text && "No text?");
	if (!SDL_SetTextureColorMod(text->Texture, r, g, b)) {
		sgLogError("Could not mod color %s", SDL_GetError());
	}
	if (!SDL_SetTextureAlphaMod(text->Texture, a)) {
		sgLogError("Could not mod alpha, %s", SDL_GetError());
	}
}

void InitializeUITextSystem(void) {
	if (!_loadedLibrary) {
		if (FT_Init_FreeType(&_loadedLibrary)) {
			sgLogCritical("Could not initialize FreeType library\n");
		}
	}
}

static void loadTexturesForFont(LoadedFont* font) {
	for (size_t i = 0; i < ASCII_CHAR_NUM; i++) {
		FT_UInt glyphIndex = FT_Get_Char_Index(font->FontFace, i);
		bool result = FT_Load_Glyph(font->FontFace, glyphIndex, FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT);
		if (result) {
			sgLogError("Could not load glyph");
		}
		int error = FT_Render_Glyph(font->FontFace->glyph,
									FT_RENDER_MODE_NORMAL);
		if (error) {
			sgLogError("Could not render font, borked");
		}
		if (font->FontFace->glyph->bitmap.pixel_mode != ft_pixel_mode_grays) {
			sgLogError("Trying to load a font that isn't grays is not supported");
		}
		if (font->FontFace->glyph->bitmap.width == 0 && font->FontFace->glyph->bitmap.rows == 0) {
			sgLogDebug("This ascii character doesn't have a width or height, skipping! %d", i);
			continue;
		}
		int pitch = font->FontFace->glyph->bitmap.pitch;
		SDL_Surface* surface = SDL_CreateSurfaceFrom(font->FontFace->glyph->bitmap.width,
													 font->FontFace->glyph->bitmap.rows,
													 SDL_PIXELFORMAT_INDEX8,
													 font->FontFace->glyph->bitmap.buffer,
													 pitch);
		if (!surface) {
			sgLogError("Bad surface: %s", SDL_GetError());
		}
		SDL_Palette* palette = SDL_CreateSurfacePalette(surface);
		int numColors = font->FontFace->glyph->bitmap.num_grays;
		for (int i = 0; i < numColors; ++i) {
			palette->colors[i].r = 255;
			palette->colors[i].g = 255;
			palette->colors[i].b = 255;
			palette->colors[i].a = (Uint8)(i);
		}
		palette->ncolors = numColors;
		result = SDL_SetPaletteColors(palette, palette->colors, 0, palette->ncolors);
		if (!result) {
			sgLogWarn("Could not set, error %s", SDL_GetError());
		}
		result = SDL_SetSurfaceColorKey(surface, true, 0);
		if (!result) {
			sgLogWarn("Could not set, error %s;", SDL_GetError());
		}
		font->GlyphTextures[i] = LoadTextureFromSurface(surface);
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
	// It isn't loaded, we should load it
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
	char fullFilepath[200];
	GetFilenameWithExeFilepath(fullFilepath, sizeof(fullFilepath), fontAndSize);
	int result = FT_New_Face(_loadedLibrary, fullFilepath, 0, &fontToLoadInto->FontFace);
	// TODO probably should cleanup here even though it may never happen
	if (result) {
		sgLogError("Could not open font %s with error %d\n", fullFilepath, result);
		return fontToLoadInto;
	}
	FT_Set_Pixel_Sizes(fontToLoadInto->FontFace, 0, size);
	loadTexturesForFont(fontToLoadInto);
	return fontToLoadInto;
}

void SetFont(const char* fontName, unsigned int size) {
	if (!fontName) {
		sgLogWarn("No font name, not setting");
		return;
	}
	if (size > 999 || size < 1) {
		sgLogWarn("Improper size passed into font, must be between 1 and 1000, setting to 32.");
		size = 32;
	}
	_currentFont = getLoadedFont(fontName, size);
}

static int getLetterYOffset(char letter, LoadedFont* font) {
	int result = FT_Load_Char(font->FontFace, letter, FT_LOAD_DEFAULT);
	if (result) {
		sgLogError("Could not measure character properly.  Char %s, error %d", letter, result);
		return 0;
	}
	return font->FontFace->glyph->bitmap_top;
}

static int getKerning(int i, char* text, LoadedFont* font) {
	if (strlen(text) <= (size_t)i + 1) {
		return 0;
	}
	if (!FT_HAS_KERNING(font->FontFace)) {
		sgLogDebug("No kerning for font, returning 0");
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

// Used for drawing, so it aligns properly
static int getLetterXOffset(UIText* text, int i) {
	int result = FT_Load_Char(text->Font->FontFace, text->Text[i], FT_LOAD_DEFAULT);
	if (result) {
		sgLogError("Could not measure character properly.  Char %s, error %d", text->Text[i], result);
		return 0;
	}
	return text->Font->FontFace->glyph->bitmap_left;
}

static int getLetterAdvance(UIText* text, int i) {
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

static bool CheckShouldWrap(int x, int wordLength, int glyphWidth, int maxX) {
	return x + wordLength + glyphWidth > maxX;
}

static void addWordToWrapPoints(unsigned int currentWordWraps, UIText* text, int location) {
	if (currentWordWraps + 1 > text->NumWordWrapCharacters) {
		unsigned int* newArray = realloc(text->WordWrapCharacters, sizeof(int) * currentWordWraps + 1);
		if (!newArray) {
			sgLogCritical("Could not allocate properly when creating text, now what");
		}
		text->WordWrapCharacters = newArray;
	}
	text->WordWrapCharacters[currentWordWraps] = location;
	++text->NumWordWrapCharacters;
}

static int getCenteredXPenLoc(UIObject* object, UIText* text) {
	int x = (object->Location.w - text->TextSizeX) / 2;
	return x < 0 ? 0 : x;
}

static int getCenteredYPenLoc(UIObject* object, UIText* text) {
	int yTop = (text->Font->FontFace->ascender * text->FontSize) / text->Font->FontFace->units_per_EM;
	int y = (object->Location.h - text->TextSizeY) / 2;
	y = yTop + y;
	return yTop < y ? y : yTop;
}

// Any time we need a full pass of the object to get the word wrap lines or size of the actual text, we will need this.
void MeasureText(UIObject* uiobject) {
	if (uiobject->Type != UIObjectTypesText) {
		return;
	}
	UIText* text = (UIText*)uiobject->Data;
	FT_Face fontFace = text->Font->FontFace;
	assert(fontFace && text && "no font loaded for text to load");
	// Max width and height will be the size of the uiobject, this should be loaded prior
	int maxWidth = uiobject->Location.w;
	int maxHeight = uiobject->Location.h;
	int currentWordLength = 0;
	int currentWordLetters = 0;
	int ascenderInPixels = (fontFace->ascender * text->FontSize) / fontFace->units_per_EM;
	int descenderInPixels = (fontFace->descender * text->FontSize) / fontFace->units_per_EM;
	int lineSpace = (fontFace->height * text->FontSize) / fontFace->units_per_EM;
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
		if (CheckShouldWrap(penX, currentWordLength, 0, maxWidth)) {
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
	if (textSizeY > maxHeight) {
		sgLogWarn("Your text overflowed through Y, please adjust your bounds else it will flow past");
	}
	if (text->CenteredX) {
		text->TextStartX = getCenteredXPenLoc(uiobject, text);
	}
}

static void drawLetter(UIObject* obj, UIText* text) {
	// Check to see if we should move to the next line:
	for (size_t i = 0; i < text->NumWordWrapCharacters; i++) {
		if (text->WordWrapCharacters[i] == text->CurrentDrawnLetters) {
			text->PenX = 0;
			if (text->CenteredX) {
				text->PenX = getCenteredXPenLoc(obj, text);
			}
			int ascenderInPixels = (text->Font->FontFace->ascender * text->FontSize) / text->Font->FontFace->units_per_EM;
			int descenderInPixels = (text->Font->FontFace->descender * text->FontSize) / text->Font->FontFace->units_per_EM;
			text->PenY += ascenderInPixels - descenderInPixels + 1;
			break;
		}
	}
	Texture* texture = text->Font->GlyphTextures[(unsigned char)text->Text[text->CurrentDrawnLetters]];
	// Draw text
	// If text is a space, and we are at beginning of a line, do not draw it }
	if (text->Text[text->CurrentDrawnLetters] == ' ' && text->WordWrap && text->PenX == text->TextStartX) {
		return;
	}
	int x = text->PenX + getLetterXOffset(text, text->CurrentDrawnLetters) - getKerning(text->CurrentDrawnLetters, text->Text, text->Font);
	int y = text->PenY - getLetterYOffset(text->Text[text->CurrentDrawnLetters], text->Font);
	float w, h;
	SDL_GetTextureSize(texture, &w, &h);
	RectangleF dst = {x, y, w, h};
	if (!text->Texture) {
		sgLogError("How is there no texture here:??");
		return;
	}
	//  If this is a drawable texture, draw it and advance
	if (texture) {
		DrawTextureToRenderTargetTexture(text->Texture, texture, &dst, NULL);
	}
	text->PenX += getLetterAdvance(text, text->CurrentDrawnLetters);
}

static void redrawText(UIObject* object, UIText* text) {
	if (text->Texture) {
		SDL_DestroyTexture(text->Texture);
	}
	text->Texture = CreateRenderTargetTexture(object->Location.w, object->Location.h, (sgColor){0, 0, 0, 0});
	if (text->Color.R != 0 || text->Color.G != 0 || text->Color.B != 0 || text->Color.A != 0) {
		SetTextColor(object, text->Color.R, text->Color.G, text->Color.B, text->Color.A);
	}
	text->PenX = 0;
	text->CurrentDrawnLetters = 0;
	text->NumWordWrapCharacters = 0;
	MeasureText(object);
	if (text->CenteredX) {
		text->PenX = getCenteredXPenLoc(object, text);
	}
	if (text->CenteredY) {
		text->PenY = getCenteredYPenLoc(object, text);

	} else {
		text->PenY = (text->Font->FontFace->ascender * text->FontSize) / text->Font->FontFace->units_per_EM;
	}
	object->Flags |= UIObjectFlagDirty;
}

void UITextOnDirty(UIObject* object) {
	if (object->Type != UIObjectTypesText) {
		return;
	}
	UIText* text = (UIText*)object->Data;
	assert(object && text && text->Font && "no font loaded for text to load");
	// If text texture is not loaded, load it.
	if (text && !text->Texture) {
		redrawText(object, text);
	}
	// Recreate texture if the size has changed
	float w, h;
	SDL_GetTextureSize(text->Texture, &w, &h);
	if (object->Location.h != h || object->Location.w != w) {
		redrawText(object, text);
	}

	// If there is more letters to draw than the current, clear the texture and start from 0
	if (text->NumLettersToDraw < text->CurrentDrawnLetters) {
		// sgColor color = {0, 255, 0, 255};
		ClearRenderTargetTexture(text->Texture, &(sgColor){0, 0, 0, 0});
		text->CurrentDrawnLetters = 0;
		text->PenX = getCenteredXPenLoc(object, text);
		text->PenY = getCenteredYPenLoc(object, text);
	}
	unsigned int lettersToWrite = text->NumLettersToDraw > strlen(text->Text) ? strlen(text->Text) : text->NumLettersToDraw;
	while (text->CurrentDrawnLetters < lettersToWrite) {
		drawLetter(object, text);
		++text->CurrentDrawnLetters;
	}
}

void UITextDraw(UIObject* object) {
	if (!object) {
		return;
	}
	UIText* text = (UIText*)object->Data;
	assert(text && "No text?");
	DrawTexture(text->Texture, &object->Location, NULL);
}

void UITextLoad(UIObject* object) {
	if (!object) {
		return;
	}
	UIText* text = (UIText*)object->Data;
	assert(text && "No text?");
	text->Font = _currentFont;
	MeasureText(object);
	if (text->CenteredX) {
		text->PenX = getCenteredXPenLoc(object, text);
	}
	if (text->CenteredY) {
		text->PenY = getCenteredYPenLoc(object, text);
	} else {
		// Set the Y to be the full ascender, so the first line doesn't get offset above.  Probably not the best place for this.
		text->PenY = (text->Font->FontFace->ascender * text->FontSize) / text->Font->FontFace->units_per_EM;
	}
}

static void setCentered(UIObject* obj, bool centered, bool x) {
	UIText* text = obj->Data;
	text->CenteredX = x ? centered : text->CenteredX;
	text->CenteredY = !x ? centered : text->CenteredY;
	redrawText(obj, text);
}

void SetCenteredX(UIObject* uiobject, int centered) {
	if (!uiobject) {
		return;
	}
	UIText* text = (UIText*)uiobject->Data;
	assert(text && "No text?");
	setCentered(uiobject, centered, true);
}

void SetCenteredY(UIObject* uiobject, int centered) {
	if (!uiobject) {
		return;
	}
	UIText* text = (UIText*)uiobject->Data;
	assert(text && "No text?");
	setCentered(uiobject, centered, false);
}

void ShutdownUITextSystem(void) {
	for (size_t i = 0; i < MAX_LOADED_FONTS; i++) {
		LoadedFont* font = &_loadedFonts[i];
		if (!font) {
			break;
		}
		SDL_free(font->FontName);
		for (size_t j = 0; j < ASCII_CHAR_NUM; j++) {
			UnloadTexture(font->GlyphTextures[j]);
		}
		FT_Done_Face(font->FontFace);
	}

	FT_Done_FreeType(_loadedLibrary);
}

void DestroyUIText(UIObject* object) {
	if (!object) {
		return;
	}
	UIText* text = (UIText*)object->Data;
	assert(text && "No text?");
	SDL_free(text->Text);
	UnloadTexture(text->Texture);
	SDL_free(text->WordWrapCharacters);
}
