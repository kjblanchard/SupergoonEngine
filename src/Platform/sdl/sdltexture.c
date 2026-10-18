// SDL_Renderer backend: textures, render targets and drawing.
//
// Ported from src/Platform/opengl/openglTexture.c.  Notable differences:
//
//  * No projection matrix.  The GL backend magnified the UI by setting the
//    viewport to the physical size while the ortho projection stayed logical;
//    here that is SDL_SetRenderScale, which SDL stores per render target.
//  * flipY is ignored.  GL framebuffers are bottom-up, SDL render targets are
//    top-down like the window, so the composite needs no flip (see sdlgraphics.c).
//  * Blend mode is chosen per draw from Texture::IsPremultiplied rather than from
//    global blend state, which is why GraphicsSetPremultipliedBlend is a no-op.
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Platform/sdl/sdlGraphics.h>
#include <Supergoon/Platform/sdl/sdlTexture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/camera.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/window.h>
#include <math.h>
#include <sgtools/log.h>
#include <sgtools/tools.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// One-deep render target stack, same as the GL backend.  Several call sites
// (map.c, nineslice.c, text.c drawLetter) rely on exactly this shape, including
// the fact that `previous` is assigned unconditionally.
static Texture* currentRenderingTarget = NULL;
static Texture* previousRenderingTarget = NULL;

static SDL_BlendMode blendModeFor(const Texture* texture) {
	return texture->IsPremultiplied ? SDL_BLENDMODE_BLEND_PREMULTIPLIED : SDL_BLENDMODE_BLEND;
}

static Texture* textureAlloc(void) {
	Texture* texture = calloc(1, sizeof(Texture));
	if (!texture) {
		sgLogError("Could not allocate texture");
	}
	return texture;
}

void TextureBindImpl(Texture* texture) {
	// SDL binds the texture as part of each draw call; nothing to do.
	(void)texture;
}

void* TextureGetIDImpl(Texture* texture) {
	if (!texture) return NULL;
	// imgui_impl_sdlrenderer3 uses SDL_Texture* directly as its ImTextureID.
	return (void*)texture->Handle;
}

int TextureGetWidthImpl(Texture* texture) { return texture->Width; }
int TextureGetHeightImpl(Texture* texture) { return texture->Height; }

Texture* TextureCreateNoCacheImpl(void) {
	// Storage is allocated later, once the pixel dimensions are known.
	return textureAlloc();
}

Texture* TextureCreateImpl(const char* name) {
	sgLogDebug("Loading new texture: %s", name);
	Texture* texture = TextureCreateNoCacheImpl();
	if (!texture) return NULL;
	texture->Name = strdup(name);
	return texture;
}

void SetRenderTargetImpl(Texture* target) {
	previousRenderingTarget = currentRenderingTarget;
	// Re-targeting forces a batch flush in SDL, and nineslice.c switches targets
	// once per 1px column, so skip the redundant work when nothing changes.
	// Observationally identical: scale/viewport/clip were set on the way in.
	if (target == currentRenderingTarget) {
		return;
	}
	if (!SDL_SetRenderTarget(_renderer, target ? target->Handle : NULL)) {
		sgLogError("Could not set render target, %s", SDL_GetError());
	}
	// SDL documents viewport/clip/scale as per-target state that persists, but
	// imgui's backend touches viewport and clip, so re-assert all three.
	SDL_SetRenderViewport(_renderer, NULL);
	SDL_SetRenderClipRect(_renderer, NULL);
	if (target && target->LogicalWidth > 0 && target->LogicalHeight > 0) {
		// This is the UI magnification: draw in logical coordinates, land on
		// physical pixels.  Replaces the GL viewport/projection mismatch.
		SDL_SetRenderScale(_renderer, (float)target->Width / (float)target->LogicalWidth,
						   (float)target->Height / (float)target->LogicalHeight);
	} else {
		SDL_SetRenderScale(_renderer, 1.0f, 1.0f);
	}
	currentRenderingTarget = target;
}

void SetPreviousRenderTargetImpl(void) { SetRenderTarget(previousRenderingTarget); }

void TextureClearRenderTargetImpl(Texture* texture, float r, float g, float b, float a) {
	SetRenderTarget(texture);
	// glClear bypassed the blend stage; SDL_RenderClear makes no such guarantee,
	// and the UI FBO and Text render targets must genuinely end up at alpha 0.
	SDL_BlendMode saved = SDL_BLENDMODE_BLEND;
	SDL_GetRenderDrawBlendMode(_renderer, &saved);
	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColorFloat(_renderer, r, g, b, a);
	if (!SDL_RenderClear(_renderer)) {
		sgLogError("Could not clear render target, %s", SDL_GetError());
	}
	SDL_SetRenderDrawBlendMode(_renderer, saved);
	SetPreviousRenderTarget();
}

Texture* TextureCreateRenderTargetImpl(int width, int height) {
	Texture* texture = textureAlloc();
	if (!texture) return NULL;
	texture->Width = width;
	texture->Height = height;
	texture->IsRenderTarget = true;
	// Compositing straight-alpha sources into a cleared target leaves the result
	// premultiplied, which is what the GL path produced too.
	texture->IsPremultiplied = true;
	asprintf(&texture->Name, "%d_%d_render_target_framebuffer", width, height);
	// An alpha channel is mandatory: the UI FBO and Text targets clear to a=0.
	texture->Handle = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32,
										SDL_TEXTUREACCESS_TARGET, width, height);
	if (!texture->Handle) {
		sgLogError("Could not create render target %dx%d, %s", width, height, SDL_GetError());
		return texture;
	}
	SDL_SetTextureScaleMode(texture->Handle, SDL_SCALEMODE_NEAREST);
	SDL_SetTextureBlendMode(texture->Handle, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
	sgLogDebug("[RT] Created render target %dx%d tex=%p", width, height, (void*)texture->Handle);
	return texture;
}

static void textureFromSurface(Texture* texture, SDL_Surface* surface) {
	texture->Handle = SDL_CreateTextureFromSurface(_renderer, surface);
	if (!texture->Handle) {
		sgLogError("Could not create texture from surface, %s", SDL_GetError());
		return;
	}
	texture->Width = surface->w;
	texture->Height = surface->h;
	texture->IsPremultiplied = false;
	// GL defaulted to GL_NEAREST; SDL defaults to LINEAR, which would blur
	// every sprite in the game.
	SDL_SetTextureScaleMode(texture->Handle, SDL_SCALEMODE_NEAREST);
	SDL_SetTextureBlendMode(texture->Handle, SDL_BLENDMODE_BLEND);
}

void TextureLoadFromPngImpl(Texture* texture, const char* filepath) {
	if (texture->Width || texture->Height) return;
	sgLogDebug("Loading from png %s", filepath);
	char* fullFilepath = NULL;
	asprintf(&fullFilepath, "%sassets/img/%s.png", GetBasePath(), filepath);
	SDL_Surface* surface = SDL_LoadPNG(fullFilepath);
	if (!surface) {
		sgLogError("Could not load png into surface, %s %s", fullFilepath, SDL_GetError());
		free(fullFilepath);
		return;
	}
	textureFromSurface(texture, surface);
	free(fullFilepath);
	SDL_DestroySurface(surface);
}

void TextureLoadFromPngBufferImpl(Texture* texture, const char* filepath, char* buf, size_t sz) {
	if (texture->Width || texture->Height) return;
	sgLogDebug("Loading from png buffer %s", filepath);
	SDL_IOStream* stream = SDL_IOFromMem(buf, sz);
	if (!stream) {
		sgLogError("Could not load stream for image, %s %s", filepath, SDL_GetError());
		return;
	}
	// Closes the stream for us, including on failure.
	SDL_Surface* surface = SDL_LoadPNG_IO(stream, true);
	if (!surface) {
		sgLogError("Could not load png into surface, %s %s", filepath, SDL_GetError());
		return;
	}
	textureFromSurface(texture, surface);
	SDL_DestroySurface(surface);
}

void TextureLoadFromDataImpl(Texture* texture, const char* name, int width, int height, void* data) {
	// FreeType hands us an 8-bit coverage bitmap.  The GL backend uploaded it as
	// GL_R8 and the text shader read .r as alpha; SDL has no single-channel
	// format, so expand to RGBA with rgb=255 and a=coverage.  Combined with
	// colormod/alphamod that reproduces 2dTextFragment.glsl exactly.
	if (texture->Name) free(texture->Name);
	texture->Name = strdup(name);
	texture->Width = width;
	texture->Height = height;
	texture->IsPremultiplied = false;
	// text.c rasterizes all 128 ASCII codes, and space and the control
	// characters come back 0x0 with a NULL buffer.  glTexImage2D accepted that
	// silently; SDL_CreateTexture(w=0) fails.  Leave Handle NULL and let
	// DrawTexture skip it.
	if (width <= 0 || height <= 0 || !data) {
		return;
	}
	unsigned char* rgba = malloc((size_t)width * (size_t)height * 4);
	if (!rgba) {
		sgLogError("Could not allocate glyph expansion buffer for %s", name);
		return;
	}
	const unsigned char* coverage = data;
	for (size_t i = 0, pixels = (size_t)width * (size_t)height; i < pixels; ++i) {
		rgba[i * 4 + 0] = 255;
		rgba[i * 4 + 1] = 255;
		rgba[i * 4 + 2] = 255;
		rgba[i * 4 + 3] = coverage[i];
	}
	texture->Handle = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32,
										SDL_TEXTUREACCESS_STATIC, width, height);
	if (!texture->Handle) {
		sgLogError("Could not create glyph texture %s (%dx%d), %s", name, width, height, SDL_GetError());
		free(rgba);
		return;
	}
	if (!SDL_UpdateTexture(texture->Handle, NULL, rgba, width * 4)) {
		sgLogError("Could not upload glyph texture %s, %s", name, SDL_GetError());
	}
	SDL_SetTextureScaleMode(texture->Handle, SDL_SCALEMODE_NEAREST);
	SDL_SetTextureBlendMode(texture->Handle, SDL_BLENDMODE_BLEND);
	free(rgba);
}

void TextureDestroyImpl(Texture* texture) {
	if (!texture) return;
	if (currentRenderingTarget == texture) currentRenderingTarget = NULL;
	if (previousRenderingTarget == texture) previousRenderingTarget = NULL;
	if (texture->Handle) {
		SDL_DestroyTexture(texture->Handle);
		texture->Handle = NULL;
	}
	if (texture->Name) free(texture->Name);
	texture->Width = 0;
	texture->Height = 0;
	free(texture);
}

static void applyColorMod(Texture* texture, Color* color) {
	SDL_SetTextureColorModFloat(texture->Handle, (float)color->R / 255.0f,
								(float)color->G / 255.0f, (float)color->B / 255.0f);
	SDL_SetTextureAlphaModFloat(texture->Handle, (float)color->A / 255.0f);
}

void DrawTextureImpl(Texture* texture, Shader* shader, RectangleF* dstRect, RectangleF* srcRect,
					 bool useCamera, float scale, bool flipY, Color* color) {
	if (!texture || !texture->Handle || !dstRect || !color) return;
	// Sprites built outside Lua have no shader (only lsprite.c assigns one), so
	// treat a missing shader as the plain sprite path rather than asserting.
	ShaderKind kind = shader ? shader->Kind : ShaderKindSprite;
	(void)flipY;	// SDL render targets are top-down; no flip needed.

	// GL floored the translation but not the size, and applied `scale` to the
	// size only.  The camera came from the view matrix as a plain translation.
	float camX = useCamera ? CameraGetX() : 0.0f;
	float camY = useCamera ? CameraGetY() : 0.0f;
	SDL_FRect dst = {
		floorf(dstRect->x) - camX,
		floorf(dstRect->y) - camY,
		dstRect->w * scale,
		dstRect->h * scale,
	};

	// The screen shader ignored srcRect entirely, and text.c depends on that: it
	// passes a logical-sized src for a physical-sized render target.
	SDL_FRect src;
	const SDL_FRect* srcPtr = NULL;
	if (kind != ShaderKindScreen && srcRect) {
		src.x = floorf(srcRect->x);
		src.y = floorf(srcRect->y);
		src.w = srcRect->w;
		src.h = srcRect->h;
		srcPtr = &src;
	}

	SDL_SetTextureBlendMode(texture->Handle, blendModeFor(texture));
	applyColorMod(texture, color);
	if (!SDL_RenderTexture(_renderer, texture->Handle, srcPtr, &dst)) {
		sgLogError("Could not draw texture %s, %s", texture->Name ? texture->Name : "?", SDL_GetError());
	}
}

void DrawTextureToScreenImpl(Texture* texture, Shader* shader, RectangleF* dstRect, bool flipY, Color* color) {
	if (!texture || !texture->Handle || !dstRect || !color) return;
	(void)shader;
	(void)flipY;
	// No camera, no floor (callers pre-floor), whole texture.  Only used to
	// composite the world and UI render targets onto the window.
	SDL_FRect dst = {dstRect->x, dstRect->y, dstRect->w, dstRect->h};
	SDL_SetTextureBlendMode(texture->Handle, blendModeFor(texture));
	applyColorMod(texture, color);
	if (!SDL_RenderTexture(_renderer, texture->Handle, NULL, &dst)) {
		sgLogError("Could not draw texture to screen, %s", SDL_GetError());
	}
}

void DrawTextureToTextureImpl(Texture* dstTarget, Texture* srcTexture, Shader* shader,
							  RectangleF* dstRect, RectangleF* srcRect, float scale) {
	SetRenderTarget(dstTarget);
	Color color = {255, 255, 255, 255};
	DrawTexture(srcTexture, shader, dstRect, srcRect, false, scale, false, &color);
	SetPreviousRenderTarget();
}
