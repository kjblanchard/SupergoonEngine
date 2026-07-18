#include <SDL3/SDL_video.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Platform/opengl/openglTexture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/camera.h>
#include <Supergoon/window.h>
#include <cglm/cglm.h>
#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
// Need to do glad first
#include <SDL3/SDL_opengl.h>
#else
#include <GLES3/gl3.h>
#include <SDL3/SDL_opengles2.h>
#endif
#include <Supergoon/Platform/opengl/openglGraphics.h>
#include <Supergoon/Platform/sdl/sdlWindow.h>
#include <sgtools/log.h>

extern void ShaderSystemShutdown(void);
extern Shader* GetDefaultScreenShaderImpl(void);
extern void DrawTextureToScreen(Texture* texture, Shader* shader, RectangleF* dstRect,
								bool flipY, Color* color);

static inline void colorToVec4(const Color* c, vec4 out) {
	out[0] = c->R / 255.0f;
	out[1] = c->G / 255.0f;
	out[2] = c->B / 255.0f;
	out[3] = c->A / 255.0f;
}

static void buildViewMatrix(mat4 view, int useCamera) {
	glm_mat4_identity(view);
	if (useCamera) {
		vec3 negCameraPos = {-CameraGetX(), -CameraGetY(), 0.0f};
		glm_translate(view, negCameraPos);
	}
}
#ifdef imgui
Texture* _imGUIScreenRenderTargetTexture = NULL;
#endif

SDL_GLContext _context;
static Texture* _screenFrameBufferTexture = NULL;
static Texture* _uiFrameBufferTexture = NULL;
// Used in debug windows
int _logicalX = 0;
int _logicalY = 0;
static GLuint vao = 0, vbo = 0;
static Color _fboColor = {255, 255, 255, 255};
#ifndef __EMSCRIPTEN__
static bool _vsync = 1;
#endif

mat4 projectionMatrix;
void GraphicsWindowResizeEventImpl(int width, int height) {
	if (!_context) {
		return;
	}
	glViewport(0, 0, width, height);
	glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, projectionMatrix);
}

void InitializeGraphicsSystemImpl(void) {
#ifdef __EMSCRIPTEN__
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);  // ES 3.0 = WebGL2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
	_context = SDL_GL_CreateContext(WindowGet()->Handle);
	if (!_context) {
		sgLogCritical("Could not create opengl context, exiting! %s",
					  SDL_GetError());
	}
#ifndef __EMSCRIPTEN__
	if (!gladLoadGL()) {
		sgLogError("Failed to initialize GLAD!");
		return;
	}
#endif
	sgLogDebug("OpenGL version: %s", glGetString(GL_VERSION));
	int width = WindowWidthImpl();
	int height = WindowHeightImpl();
	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm_ortho(0.0f, WindowWidthImpl(), WindowHeightImpl(), 0.0f, -1.0f, 1.0f,
			  projectionMatrix);
#ifndef __EMSCRIPTEN__
	SDL_GL_SetSwapInterval(_vsync);	 // vsync
#endif
	//Setup the reusable VAO and make it with the VBO.
	float verts[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f};
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

void ShutdownGraphicsSystemImpl(void) {
	ShaderSystemShutdown();
	SDL_GL_DestroyContext(_context);
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

void DrawUIStartImpl(void) {
	SetRenderTarget(_uiFrameBufferTexture);
}

void DrawEndImpl(void) {
	SetRenderTarget(NULL);
#ifdef imgui
	SetRenderTarget(_imGUIScreenRenderTargetTexture);
#endif
	if (!_screenFrameBufferTexture) {
		SDL_GL_SwapWindow(WindowGetImpl()->Handle);
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
	float subX = floorf(CameraGetSubPixelX() * scale);
	float subY = floorf(CameraGetSubPixelY() * scale);
	Color fboColor = _fboColor;

	float worldX = offsetX - subX;
	float worldY = offsetY + subY;
	RectangleF worldDst = {worldX, worldY, (float)drawWidth, (float)drawHeight};
	Shader* screenShader = GetDefaultScreenShaderImpl();
	DrawTextureToScreen(_screenFrameBufferTexture, screenShader, &worldDst, true, &fboColor);

	if (_uiFrameBufferTexture) {
		RectangleF uiDst = {offsetX, offsetY, (float)drawWidth, (float)drawHeight};
		DrawTextureToScreen(_uiFrameBufferTexture, screenShader, &uiDst, true, &fboColor);
	}
#ifdef imgui
	SetRenderTarget(NULL);
#endif
	if (GraphicsPostFBODrawDebugFunc) GraphicsPostFBODrawDebugFunc();
	SDL_GL_SwapWindow(WindowGetImpl()->Handle);
}

void DrawLineImpl(float x1, float y1, float x2, float y2, float thickness, Color* color, int useCamera) {
	Shader* shader = GetDefaultRectShader();
	ShaderUse(shader);
	// Compute direction and length
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = sqrtf(dx * dx + dy * dy);
	float angle = atan2f(dy, dx);
	// Build model matrix
	mat4 model;
	glm_mat4_identity(model);
	// Translate to starting point
	glm_translate(model, (vec3){x1, y1, 0.0f});
	// Rotate to match direction
	glm_rotate(model, angle, (vec3){0.0f, 0.0f, 1.0f});
	// Scale to line length and thickness
	glm_scale(model, (vec3){length, thickness, 1.0f});
	vec4 colorV;
	colorToVec4(color, colorV);
	mat4 view;
	buildViewMatrix(view, useCamera);
	ShaderSetUniformMatrix4(shader, "projection", projectionMatrix, false);
	ShaderSetUniformMatrix4(shader, "model", model, false);
	ShaderSetUniformMatrix4(shader, "view", view, false);
	ShaderSetUniformVector4fV(shader, "color", colorV, false);
	glBindVertexArray(vao);
	// Draw as filled quad
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);
}

void DrawRectImpl(RectangleF* rect, Color* color, int filled, int useCamera) {
	Shader* shader = GetDefaultRectShader();
	ShaderUse(shader);
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){rect->x, rect->y, 0.0f});
	glm_scale(model, (vec3){rect->w, rect->h, 1.0f});
	vec4 colorV;
	colorToVec4(color, colorV);
	mat4 view;
	buildViewMatrix(view, useCamera);
	ShaderSetUniformMatrix4(shader, "projection", projectionMatrix, false);
	ShaderSetUniformMatrix4(shader, "model", model, false);
	ShaderSetUniformVector4fV(shader, "color", colorV, false);
	ShaderSetUniformMatrix4(shader, "view", view, false);
	glBindVertexArray(vao);
	if (filled) {
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	} else {
		glDrawArrays(GL_LINE_STRIP, 0, 5);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

void GraphicsSetLogicalWorldSizeImpl(int width, int height) {
	_logicalX = width;
	_logicalY = height;
	glViewport(0, 0, width, height);
	glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, projectionMatrix);
	if (_screenFrameBufferTexture) {
		TextureDestroy(_screenFrameBufferTexture);
	}
	_screenFrameBufferTexture = TextureCreateRenderTarget(width, height);
	TextureClearRenderTarget(_screenFrameBufferTexture, 0, 0, 0, 1.0);
	if (_uiFrameBufferTexture) {
		TextureDestroy(_uiFrameBufferTexture);
	}
	_uiFrameBufferTexture = TextureCreateRenderTarget(width, height);
	TextureClearRenderTarget(_uiFrameBufferTexture, 0, 0, 0, 0.0);
#ifdef imgui
	if (_imGUIScreenRenderTargetTexture) {
		TextureDestroy(_imGUIScreenRenderTargetTexture);
	}
	_imGUIScreenRenderTargetTexture = TextureCreateRenderTarget(width, height);
	TextureClearRenderTarget(_imGUIScreenRenderTargetTexture, 0, 0, 0, 1.0);
#endif
}

void GraphicsUpdateFBOColorImpl(Color* color) {
	_fboColor = *color;
}
Color GraphicsGetFBOColorImpl(void) {
	return _fboColor;
}

void* GraphicsGetContextPtrImpl(void) {
	return _context;
}
