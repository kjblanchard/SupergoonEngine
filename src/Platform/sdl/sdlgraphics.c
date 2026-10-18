// SDL_Renderer backend: renderer lifecycle, render targets and the frame composite.
//
// Ported from src/Platform/opengl/openglGraphics.c.  The three-render-target
// architecture is unchanged:
//
//   _screenFrameBufferTexture        logical size (e.g. 480x270), the world
//   _uiFrameBufferTexture            logical * uiScale, drawn in logical coords
//   _imGUIScreenRenderTargetTexture  logical * uiScale, what imgui samples
//
// DrawEnd integer-scales and centers those onto the window, exactly as before.
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Platform/sdl/sdlGraphics.h>
#include <Supergoon/Platform/sdl/sdlTexture.h>
#include <Supergoon/Platform/sdl/sdlWindow.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/UI/object.h>
#include <Supergoon/camera.h>
#include <Supergoon/state.h>
#include <Supergoon/window.h>
#include <math.h>
#include <sgtools/log.h>
#ifdef imgui
#include <DebugWindow.hpp>
#endif

SDL_Renderer* _renderer = NULL;

// Read via `extern` by src/Input/mouse.c to map window coords back to game
// coords, and used by the debug windows.  Must not be static.
int _logicalX = 0;
int _logicalY = 0;

#ifdef imgui
// DebugGameWindow.cpp externs this by name.
Texture* _imGUIScreenRenderTargetTexture = NULL;
#endif

static Texture* _screenFrameBufferTexture = NULL;
static Texture* _uiFrameBufferTexture = NULL;
// Defaults to 1 because text.c reads it to pick a FreeType pixel size, possibly
// before GraphicsSetLogicalWorldSize has run.
static int _uiScale = 1;
static Color _fboColor = {255, 255, 255, 255};

extern void ShaderSystemShutdown(void);
extern Shader* GetDefaultScreenShaderImpl(void);

void InitializeGraphicsSystemImpl(void) {
	_renderer = SDL_CreateRenderer(WindowGet()->Handle, NULL);
	if (!_renderer) {
		sgLogCritical("Could not create SDL renderer, exiting! %s", SDL_GetError());
		return;
	}
	sgLogDebug("[GFX INIT] SDL renderer created: %s", SDL_GetRendererName(_renderer));
	sgLogDebug("[GFX INIT] Window pixel size: %d x %d", WindowWidthImpl(), WindowHeightImpl());
	if (!SDL_SetRenderVSync(_renderer, 1)) {
		sgLogWarn("Could not enable vsync, %s", SDL_GetError());
	}
	// Matches the GL default glBlendFuncSeparate(SRC_ALPHA, 1-SRC_ALPHA, ONE, 1-SRC_ALPHA).
	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	// Deliberately no SDL_SetRenderLogicalPresentation: the engine letterboxes by
	// hand in DrawEnd and mouse.c duplicates that math, so window coordinates
	// must stay raw pixels.
}

void ShutdownGraphicsSystemImpl(void) {
	ShaderSystemShutdown();
	if (_renderer) {
		SDL_DestroyRenderer(_renderer);
		_renderer = NULL;
	}
}

void GraphicsWindowResizeEventImpl(int width, int height) {
	// CreateWindow calls this through onWindowResize before the renderer exists.
	if (!_renderer) {
		return;
	}
	(void)width;
	(void)height;
	if (_logicalX > 0 && _logicalY > 0) {
		GraphicsSetLogicalWorldSize(_logicalX, _logicalY);
		if (RootUIObject) UIObjectDirty(RootUIObject);
	}
}

void GraphicsSetLogicalWorldSizeImpl(int width, int height) {
	sgLogDebug("[GFX] SetLogicalWorldSize: %d x %d", width, height);
	_logicalX = width;
	_logicalY = height;
	int winW = WindowWidth();
	int winH = WindowHeight();
	int scaleX = winW / width;
	int scaleY = winH / height;
	_uiScale = scaleX < scaleY ? scaleX : scaleY;
	if (_uiScale < 1) _uiScale = 1;
	sgLogDebug("[GFX] UI scale: %d (win %dx%d, logical %dx%d)", _uiScale, winW, winH, width, height);

	// World target: logical resolution, nearest-sampled when blitted up.
	if (_screenFrameBufferTexture) {
		TextureDestroy(_screenFrameBufferTexture);
	}
	_screenFrameBufferTexture = TextureCreateRenderTarget(width, height);
	sgLogDebug("[GFX] Screen FBO created: texture=%p", (void*)_screenFrameBufferTexture);
	TextureClearRenderTarget(_screenFrameBufferTexture, 0, 0, 0, 1.0f);

	// UI target: physical resolution, but drawn in logical coordinates.  Setting
	// LogicalWidth/Height before the render scale is applied is what makes that
	// work, so assign them before the first SetRenderTarget.
	if (_uiFrameBufferTexture) {
		TextureDestroy(_uiFrameBufferTexture);
	}
	int uiPhysW = width * _uiScale;
	int uiPhysH = height * _uiScale;
	_uiFrameBufferTexture = TextureCreateRenderTarget(uiPhysW, uiPhysH);
	_uiFrameBufferTexture->LogicalWidth = width;
	_uiFrameBufferTexture->LogicalHeight = height;
	if (_uiFrameBufferTexture->Handle) {
		SDL_SetTextureScaleMode(_uiFrameBufferTexture->Handle, SDL_SCALEMODE_LINEAR);
	}
	sgLogDebug("[GFX] UI FBO created: %dx%d physical, %dx%d logical", uiPhysW, uiPhysH, width, height);
	TextureClearRenderTarget(_uiFrameBufferTexture, 0, 0, 0, 0.0f);

#ifdef imgui
	if (_imGUIScreenRenderTargetTexture) {
		TextureDestroy(_imGUIScreenRenderTargetTexture);
	}
	// Physical resolution with LogicalWidth/Height left at 0, so the UI FBO
	// composites into it 1:1 with no downscale.
	_imGUIScreenRenderTargetTexture = TextureCreateRenderTarget(uiPhysW, uiPhysH);
	if (_imGUIScreenRenderTargetTexture->Handle) {
		SDL_SetTextureScaleMode(_imGUIScreenRenderTargetTexture->Handle, SDL_SCALEMODE_LINEAR);
	}
	TextureClearRenderTarget(_imGUIScreenRenderTargetTexture, 0.1f, 0.1f, 0.1f, 1.0f);
#endif
}

void DrawStartImpl(void) {
	TextureClearRenderTarget(NULL, 0.1f, 0.1f, 0.1f, 1.0f);
	TextureClearRenderTarget(_screenFrameBufferTexture, 0.1f, 0.1f, 0.1f, 1.0f);
	TextureClearRenderTarget(_uiFrameBufferTexture, 0.0f, 0.0f, 0.0f, 0.0f);
#ifdef imgui
	TextureClearRenderTarget(_imGUIScreenRenderTargetTexture, 0.1f, 0.1f, 0.1f, 1.0f);
#endif
	SetRenderTarget(_screenFrameBufferTexture);
}

void DrawUIStartImpl(void) { SetRenderTarget(_uiFrameBufferTexture); }

static int _drawEndLogCount = 0;
void DrawEndImpl(void) {
	SetRenderTarget(NULL);
#ifdef imgui
	// Only builds the imgui draw list; it issues no draw calls, so the current
	// render target does not matter here.  The ImGui::Image in DebugGameWindow
	// records the render target's SDL_Texture*, which is filled in just below
	// and sampled later in DebugSystemRender.
	DebugSystemDraw();
	SetRenderTarget(_imGUIScreenRenderTargetTexture);
#endif
	if (!_screenFrameBufferTexture) {
		if (_drawEndLogCount < 3) sgLogWarn("[DRAW] DrawEnd: no screen FBO texture, just presenting");
		// SDL_RenderPresent fails while a texture target is bound, so unbind
		// first.  The GL version got away with swapping mid-target.
		SetRenderTarget(NULL);
		SDL_RenderPresent(_renderer);
		return;
	}
	int fbWidth = TextureGetWidth(_screenFrameBufferTexture);
	int fbHeight = TextureGetHeight(_screenFrameBufferTexture);
#ifdef imgui
	int winWidth = TextureGetWidth(_imGUIScreenRenderTargetTexture);
	int winHeight = TextureGetHeight(_imGUIScreenRenderTargetTexture);
#else
	int winWidth = WindowWidth();
	int winHeight = WindowHeight();
#endif
	int scaleX = winWidth / fbWidth;
	int scaleY = winHeight / fbHeight;
	int scale = scaleX < scaleY ? scaleX : scaleY;
	if (scale < 1) scale = 1;
	int drawWidth = fbWidth * scale;
	int drawHeight = fbHeight * scale;
	float offsetX = floorf((winWidth - drawWidth) / 2.0f);
	float offsetY = floorf((winHeight - drawHeight) / 2.0f);
	if (_drawEndLogCount < 3) {
		sgLogDebug("[DRAW] fb=%dx%d win=%dx%d scale=%d draw=%dx%d offset=%.0f,%.0f",
				   fbWidth, fbHeight, winWidth, winHeight, scale, drawWidth, drawHeight, offsetX, offsetY);
		++_drawEndLogCount;
	}
	float subX = floorf(CameraGetSubPixelX() * scale);
	float subY = floorf(CameraGetSubPixelY() * scale);
	Color fboColor = _fboColor;

	// The GL projection for render targets was Y-UP (glm_ortho(0,w,0,h,...)) and
	// the flipY=true on these two composites is what cancelled it, so GL's
	// `offsetY + subY` is a Y-up expression.  In top-down SDL space the subpixel
	// term flips sign.  X is never flipped, and the centering offsets are
	// symmetric, so only this one term changes.
	float worldX = offsetX - subX;
	float worldY = offsetY - subY;
	RectangleF worldDst = {worldX, worldY, (float)drawWidth, (float)drawHeight};
	Shader* screenShader = GetDefaultScreenShaderImpl();
	DrawTextureToScreen(_screenFrameBufferTexture, screenShader, &worldDst, false, &fboColor);

	if (_uiFrameBufferTexture) {
		RectangleF uiDst = {offsetX, offsetY, (float)drawWidth, (float)drawHeight};
		DrawTextureToScreen(_uiFrameBufferTexture, screenShader, &uiDst, false, &fboColor);
	}
#ifdef imgui
	// Must be the window, and at scale 1, before imgui draws: its backend
	// restores viewport and clip but not the render target or the scale.
	SetRenderTarget(NULL);
	DebugSystemRender();
#endif
	SDL_RenderPresent(_renderer);
}

void DrawRectImpl(const RectangleF* rect, Color* color, int filled, int useCamera) {
	if (!_renderer || !rect || !color) return;
	float camX = useCamera ? CameraGetX() : 0.0f;
	float camY = useCamera ? CameraGetY() : 0.0f;
	SDL_FRect dst = {rect->x - camX, rect->y - camY, rect->w, rect->h};
	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColorFloat(_renderer, (float)color->R / 255.0f, (float)color->G / 255.0f,
								(float)color->B / 255.0f, (float)color->A / 255.0f);
	bool ok = filled ? SDL_RenderFillRect(_renderer, &dst) : SDL_RenderRect(_renderer, &dst);
	if (!ok) {
		sgLogError("Could not draw rect, %s", SDL_GetError());
	}
}

void DrawLineImpl(float x1, float y1, float x2, float y2, float thickness, Color* color, int useCamera) {
	if (!_renderer || !color) return;
	// Always geometry, never SDL_RenderLine: the GL version built a thickness
	// quad, so thickness is in logical units and must be magnified by the render
	// scale.  SDL_RenderLine would always be one physical pixel instead.
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = sqrtf(dx * dx + dy * dy);
	float angle = atan2f(dy, dx);
	float c = cosf(angle);
	float s = sinf(angle);
	float camX = useCamera ? CameraGetX() : 0.0f;
	float camY = useCamera ? CameraGetY() : 0.0f;
	// Unit quad scaled to length x thickness, rotated, then translated - the same
	// transform the GL model matrix applied.
	const float localX[4] = {0.0f, length, length, 0.0f};
	const float localY[4] = {0.0f, 0.0f, thickness, thickness};
	SDL_FColor fc = {(float)color->R / 255.0f, (float)color->G / 255.0f,
					 (float)color->B / 255.0f, (float)color->A / 255.0f};
	SDL_Vertex verts[4];
	for (int i = 0; i < 4; ++i) {
		verts[i].position.x = x1 + localX[i] * c - localY[i] * s - camX;
		verts[i].position.y = y1 + localX[i] * s + localY[i] * c - camY;
		verts[i].color = fc;
		verts[i].tex_coord.x = 0.0f;
		verts[i].tex_coord.y = 0.0f;
	}
	const int indices[6] = {0, 1, 2, 0, 2, 3};
	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	if (!SDL_RenderGeometry(_renderer, NULL, verts, 4, indices, 6)) {
		sgLogError("Could not draw line, %s", SDL_GetError());
	}
}

int GraphicsGetUIScaleImpl(void) { return _uiScale; }

void GraphicsUpdateFBOColorImpl(Color* color) { _fboColor = *color; }
Color GraphicsGetFBOColorImpl(void) { return _fboColor; }

void* GraphicsGetContextPtrImpl(void) { return _renderer; }

void GraphicsSetPremultipliedBlendImpl(int premultiplied) {
	// No-op by design.  Blending now follows whether the *source texture* holds
	// premultiplied pixels (Texture::IsPremultiplied), which is strictly more
	// accurate than a global flag.  Honoring this literally would double-darken
	// text, because SDL glyph textures are straight-alpha while the GL text
	// shader premultiplied in the fragment stage.
	(void)premultiplied;
}
