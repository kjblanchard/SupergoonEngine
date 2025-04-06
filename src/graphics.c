#include <SDL3/SDL.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
#include <SupergoonEngine/graphics.h>
#include <SupergoonEngine/tools.h>
#include <assert.h>
#include <stdlib.h>

// defined in window.c
extern Window* _window;
extern Renderer* _renderer;

TextureCacheItem* _textureCache = NULL;
bool _holes = false;
size_t _numTexturesInCache = 0;
static size_t _firstHole = 0;
static size_t _textureCacheSize = 8;

static Texture* getTextureFromCache(const char* name) {
	for (size_t i = 0; i < _numTexturesInCache; i++) {
		if (strcmp(name, _textureCache[i].name) == 0) {
			++_textureCache[i].References;
			return _textureCache[i].Texture;
		}
	}
	return NULL;
}

static void addTextureToCache(Texture* texture, const char* name) {
	size_t insertLocation = _holes ? _firstHole : _numTexturesInCache;
	if (_holes) {
		for (size_t i = _firstHole + 1; i < _numTexturesInCache; i++) {
			if (_textureCache[i].References == 0) {
				_firstHole = i;
				break;
			}
		}
		if (_firstHole == insertLocation) {
			_holes = false;
			_firstHole = 0;
		}
	}
	if (_numTexturesInCache > _textureCacheSize / 2) {
		RESIZE_ARRAY(_textureCache, _textureCacheSize, TextureCacheItem);
	}
	_textureCache[insertLocation].References = 1;
	_textureCache[insertLocation].Texture = texture;
	_textureCache[insertLocation].name = strdup(name);
	if (insertLocation == _numTexturesInCache) {
		++_numTexturesInCache;
	}
}

Texture* LoadTextureFromSurface(struct SDL_Surface* surface) {
	Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
	if (!texture) {
		sgLogError("Could not create texture from surface %s!", SDL_GetError());
	}
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	return texture;
}

void ClearRenderTargetTexture(Texture* texture, sgColor* color) {
	SDL_Texture* currentRenderTarget = SDL_GetRenderTarget(_renderer);
	if (!SDL_SetRenderTarget(_renderer, texture)) {
		sgLogError("Could not set renderer to texture target to clear");
		return;
	}
	SDL_RenderClear(_renderer);
	SDL_SetRenderTarget(_renderer, currentRenderTarget);
}

void DrawRect(RectangleF* rect) {
	assert(_renderer && "No renderer, make sure window is created");
	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
	SDL_RenderRect(_renderer, rect);
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
}

Texture* CreateRenderTargetTexture(int width, int height, sgColor color) {
	assert(_renderer && "No renderer, make sure window is created");
	Texture* image = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	if (!image) {
		sgLogError("Error creating image, %s", SDL_GetError());
	}
	if (!SDL_SetRenderTarget(_renderer, image)) {
		sgLogError("Error setting render target when creating render target texture, %s", SDL_GetError());
	}
	if (!SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND)) {
		sgLogError("Error setting blend mode: %s", SDL_GetError());
	}
	SDL_SetTextureScaleMode(image, SDL_SCALEMODE_NEAREST);
	SDL_SetTextureBlendMode(image, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(_renderer, color.R, color.G, color.B, color.A);
	SDL_RenderClear(_renderer);
	SDL_SetRenderTarget(_renderer, NULL);
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	return image;
}

Texture* CreateTextureFromIndexedBMP(const char* filename) {
	SDL_Texture* texture = getTextureFromCache(filename);
	if (texture) {
		return texture;
	}
	char name[50];
	char fullFilepath[200];
	snprintf(name, sizeof(name), "assets/img/%s.bmp", filename);
	GetFilenameWithExeFilepath(fullFilepath, sizeof(fullFilepath), name);
	SDL_Surface* surface = SDL_LoadBMP(fullFilepath);
	if (!surface) {
		sgLogError("Could not load bmp into surface, %s %s", fullFilepath, SDL_GetError());
		return NULL;
	}
	SDL_Color transparentColor = {255, 0, 255, 255};
	if (!SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, transparentColor.r, transparentColor.g, transparentColor.b))) {
		printf("Failed to set color key: %s\n", SDL_GetError());
	}

	texture = SDL_CreateTextureFromSurface(_renderer, surface);
	if (!texture) {
		sgLogError("Could not load texture from surface, %s: %s", fullFilepath, SDL_GetError());
		return NULL;
	}
	addTextureToCache(texture, filename);
	return texture;
}
void DrawTextureToRenderTargetTexture(Texture* dst, Texture* src, RectangleF* dstRect, RectangleF* srcRect) {
	Texture* currentRenderTarget = SDL_GetRenderTarget(_renderer);
	bool result = SDL_SetRenderTarget(_renderer, dst);
	if (!result) {
		sgLogError("Could not set render target, %s", SDL_GetError());
	}
	result = SDL_RenderTexture(_renderer, src, srcRect, dstRect);
	if (!result) {
		sgLogError("Bad draw, %s", SDL_GetError());
	}
	SDL_SetRenderTarget(_renderer, currentRenderTarget);
}

static void unloadTexture(int i) {
	SDL_DestroyTexture(_textureCache[i].Texture);
	SDL_free(_textureCache[i].name);
	_textureCache[i].name = NULL;
	_textureCache[i].Texture = NULL;
}

void UnloadTexture(Texture* texture) {
	for (size_t i = 0; i < _numTexturesInCache; i++) {
		if (_textureCache[i].References && _textureCache[i].Texture == texture) {
			--_textureCache[i].References;
			if (_textureCache[i].References == 0) {
				_holes = true;
			}
			return;
		}
	}
	sgLogWarn("Trying to unload a texture not in the cache, how even: %d", texture);
}

void UnloadUnusedTextures(void) {
	if (!_holes) {
		return;
	}
	for (size_t i = 0; i < _numTexturesInCache; i++) {
		if (_textureCache[i].References == 0) {
			unloadTexture(i);
		}
	}
	_holes = false;
}

void UnloadAllTextures(void) {
	for (size_t i = 0; i < _numTexturesInCache; i++) {
		unloadTexture(i);
	}
	_holes = false;
}

void InitializeGraphicsSystem(void) {
	RESIZE_ARRAY(_textureCache, _textureCacheSize, TextureCacheItem);
}
