#pragma once
#include <SDL3/SDL_render.h>

#ifdef __cplusplus
extern "C" {
#endif

// SDL_Renderer has no shaders.  The four "default shaders" survive only as tags,
// because DrawTexture still has to know whether to honor srcRect: the screen
// shader ignored it, and text.c relies on that (it passes a logical-sized src for
// a physical-sized render target).
typedef enum ShaderKind {
	ShaderKindSprite,
	ShaderKindText,
	ShaderKindRect,
	ShaderKindScreen,
} ShaderKind;

typedef struct Shader {
	ShaderKind Kind;
} Shader;

extern SDL_Renderer* _renderer;

#ifdef __cplusplus
}
#endif
