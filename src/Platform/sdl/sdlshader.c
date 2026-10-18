// SDL_Renderer backend: shader stubs.
//
// SDL_Renderer offers no programmable pipeline, and the four shaders this engine
// actually used were trivial enough to fold into SDL's fixed-function state:
//
//   2dSprite  -> SDL_RenderTexture + colormod/alphamod, SDL_BLENDMODE_BLEND
//   2dText    -> same, over a glyph expanded to RGBA (rgb=255, a=coverage)
//   2dRect    -> SDL_RenderFillRect / SDL_RenderRect
//   2dScreen  -> SDL_RenderTexture with srcrect=NULL, BLEND_PREMULTIPLIED
//
// So a Shader is just a tag telling DrawTexture which of those to emulate.  The
// compile and uniform entry points remain because text.c, map.c, nineslice.c and
// the Lua bindings still call the public API; they are no-ops here.
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Platform/sdl/sdlGraphics.h>
#include <sgtools/log.h>
#include <stdlib.h>

static Shader _defaultShader = {ShaderKindSprite};
static Shader _defaultTextShader = {ShaderKindText};
static Shader _defaultRectShader = {ShaderKindRect};
static Shader _defaultScreenShader = {ShaderKindScreen};
static Shader* _screenShaderOverride = NULL;

Shader* GetDefaultShaderImpl(void) { return &_defaultShader; }
Shader* GetDefaultTextShaderImpl(void) { return &_defaultTextShader; }
Shader* GetDefaultRectShaderImpl(void) { return &_defaultRectShader; }

Shader* GetDefaultScreenShaderImpl(void) {
	if (_screenShaderOverride) {
		return _screenShaderOverride;
	}
	return &_defaultScreenShader;
}

void SetScreenShaderOverrideImpl(Shader* s) { _screenShaderOverride = s; }
Shader* GetScreenShaderOverrideImpl(void) { return _screenShaderOverride; }

// A caller-created shader has no program to back it, so it behaves as a sprite.
Shader* ShaderCreateImpl(void) {
	Shader* shader = malloc(sizeof(Shader));
	if (!shader) {
		sgLogError("Could not allocate shader");
		return NULL;
	}
	shader->Kind = ShaderKindSprite;
	return shader;
}

void ShaderDestroyImpl(Shader* shader) {
	// The four defaults are static; only free heap-allocated ones.
	if (!shader) return;
	if (shader == &_defaultShader || shader == &_defaultTextShader ||
		shader == &_defaultRectShader || shader == &_defaultScreenShader) {
		return;
	}
	free(shader);
}

void ShaderUseImpl(Shader* shader) { (void)shader; }

void ShaderSystemShutdown(void) { _screenShaderOverride = NULL; }

static void warnOnce(const char* what) {
	static int warned = 0;
	if (warned) return;
	warned = 1;
	sgLogWarn("%s has no effect under the SDL_Renderer backend (no shader support)", what);
}

void ShaderCompileImpl(Shader* shader, const char* vertexSourceFile,
					   const char* fragmentSourceFile) {
	(void)shader;
	(void)vertexSourceFile;
	(void)fragmentSourceFile;
	warnOnce("ShaderCompile");
}

void ShaderSetUniformFloatImpl(Shader* shader, const char* name, float value, int useShader) {
	(void)shader;
	(void)name;
	(void)value;
	(void)useShader;
	warnOnce("ShaderSetUniform*");
}

void ShaderSetUniformIntegerImpl(Shader* shader, const char* name, int value, int useShader) {
	(void)shader;
	(void)name;
	(void)value;
	(void)useShader;
	warnOnce("ShaderSetUniform*");
}

void ShaderSetUniformVector2fImpl(Shader* shader, const char* name, float x, float y, int useShader) {
	(void)shader;
	(void)name;
	(void)x;
	(void)y;
	(void)useShader;
	warnOnce("ShaderSetUniform*");
}

void ShaderSetUniformVector2fVImpl(Shader* shader, const char* name, vec2 value, int useShader) {
	(void)shader;
	(void)name;
	(void)value;
	(void)useShader;
	warnOnce("ShaderSetUniform*");
}

void ShaderSetUniformVector3fImpl(Shader* shader, const char* name, float x, float y, float z, int useShader) {
	(void)shader;
	(void)name;
	(void)x;
	(void)y;
	(void)z;
	(void)useShader;
	warnOnce("ShaderSetUniform*");
}

void ShaderSetUniformVector3fVImpl(Shader* shader, const char* name, vec3 value, int useShader) {
	(void)shader;
	(void)name;
	(void)value;
	(void)useShader;
	warnOnce("ShaderSetUniform*");
}

void ShaderSetUniformVector4fImpl(Shader* shader, const char* name, float x, float y, float z, float w, int useShader) {
	(void)shader;
	(void)name;
	(void)x;
	(void)y;
	(void)z;
	(void)w;
	(void)useShader;
	warnOnce("ShaderSetUniform*");
}

void ShaderSetUniformVector4fVImpl(Shader* shader, const char* name, vec4 value, int useShader) {
	(void)shader;
	(void)name;
	(void)value;
	(void)useShader;
	warnOnce("ShaderSetUniform*");
}

void ShaderSetUniformMatrix4Impl(Shader* shader, const char* name, mat4 value, int useShader) {
	(void)shader;
	(void)name;
	(void)value;
	(void)useShader;
	warnOnce("ShaderSetUniform*");
}
