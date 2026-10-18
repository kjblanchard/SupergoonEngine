#pragma once
#include <SDL3/SDL_render.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Texture {
	// NULL until pixel data is loaded, or for zero-sized glyphs (space, control chars).
	SDL_Texture* Handle;
	int Width;
	int Height;
	// When set, drawing into this target uses logical coordinates and SDL scales
	// them up to the physical Width/Height.  0 means draw in physical pixels.
	int LogicalWidth;
	int LogicalHeight;
	bool IsRenderTarget;
	// Render targets hold premultiplied RGB (they are composited into a cleared
	// target), PNG and glyph textures hold straight alpha.  Picks the blend mode.
	bool IsPremultiplied;
	char* Name;
} Texture;

#ifdef __cplusplus
}
#endif
