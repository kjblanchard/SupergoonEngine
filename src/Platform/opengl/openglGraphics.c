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
SDL_GLContext _context;
static Texture* _screenFrameBufferTexture = NULL;
static int _logicalX = 0;
static GLuint vao = 0, vbo = 0;
static Color _fboColor = {255, 255, 255, 255};
static int _logicalY = 0;
// TODO for now, only use the refresh rate set here.. we should set it eventually.
static unsigned int _refreshRate = 999;
/* #ifndef __EMSCRIPTEN__ */
static bool _vsync = 1;
/* #endif */

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
	/* #ifndef __EMSCRIPTEN__ */
	SDL_GL_SetSwapInterval(_vsync);	 // vsync
									 /* #endif */
	// Try to use the thing
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
	SetRenderTarget(NULL);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SetRenderTarget(_screenFrameBufferTexture);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void DrawEndImpl(void) {
	SetRenderTarget(NULL);
	if (!_screenFrameBufferTexture) {
		SDL_GL_SwapWindow(WindowGetImpl()->Handle);
		return;
	}
	int fbWidth = TextureGetWidth(_screenFrameBufferTexture);
	int fbHeight = TextureGetHeight(_screenFrameBufferTexture);
	int winWidth = WindowWidth();
	int winHeight = WindowHeight();
	int scaleX = winWidth / fbWidth;
	int scaleY = winHeight / fbHeight;
	int scale = scaleX < scaleY ? scaleX : scaleY;
	if (scale < 1) scale = 1;
	int drawWidth = fbWidth * scale;
	int drawHeight = fbHeight * scale;
	float offsetX = (winWidth - drawWidth) / 2.0f;
	float offsetY = (winHeight - drawHeight) / 2.0f;
	float subX = CameraGetSubPixelX();
	float subY = CameraGetSubPixelY();
	float dstX = offsetX - subX * scale;
	float dstY = offsetY - subY * scale;

	Shader* shader = GetDefaultShader();
	ShaderUse(shader);
	mat4 model;
	glm_mat4_identity(model);
	vec3 pos = {dstX, dstY + drawHeight, 0};
	glm_translate(model, pos);
	vec3 size = {(float)drawWidth, -(float)drawHeight, 1.0f};
	glm_scale(model, size);
	mat4 view;
	glm_mat4_identity(view);
	vec4 srcRectV = {0, 0, (float)fbWidth, (float)fbHeight};
	vec2 texSize = {(float)fbWidth, (float)fbHeight};
	ShaderSetUniformVector4fV(shader, "srcRect", srcRectV, false);
	ShaderSetUniformVector2fV(shader, "textureSize", texSize, false);
	ShaderSetUniformMatrix4(shader, "model", model, false);
	ShaderSetUniformMatrix4(shader, "view", view, false);
	ShaderSetUniformMatrix4(shader, "projection", projectionMatrix, false);
	ShaderSetUniformInteger(shader, "image", 0, false);
	vec4 colorVec = {_fboColor.R / 255.0f, _fboColor.G / 255.0f, _fboColor.B / 255.0f, _fboColor.A / 255.0f};
	ShaderSetUniformVector4fV(shader, "spriteColor", colorVec, false);
	glActiveTexture(GL_TEXTURE0);
	TextureBind(_screenFrameBufferTexture);
	glBindVertexArray(_screenFrameBufferTexture->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	if (GraphicsPostFBODrawUIFunc) {
		glViewport((int)offsetX, (int)offsetY, drawWidth, drawHeight);
		glm_ortho(0.0f, (float)fbWidth, (float)fbHeight, 0.0f, -1.0f, 1.0f, projectionMatrix);
		GraphicsPostFBODrawUIFunc();
		glViewport(0, 0, winWidth, winHeight);
		glm_ortho(0.0f, (float)winWidth, 0.0f, (float)winHeight, -1.0f, 1.0f, projectionMatrix);
	}

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
	glm_translate(model, (vec3){floorf(x1), floorf(y1), 0.0f});

	// Rotate to match direction
	glm_rotate(model, angle, (vec3){0.0f, 0.0f, 1.0f});

	// Scale to line length and thickness
	glm_scale(model, (vec3){length, thickness, 1.0f});

	// Color
	vec4 colorV = {
		color->R / 255.0f,
		color->G / 255.0f,
		color->B / 255.0f,
		color->A / 255.0f};

	// View matrix
	mat4 view;
	glm_mat4_identity(view);
	if (useCamera) {
		float cx = CameraGetX();
		float cy = CameraGetY();
		float subX = CameraGetSubPixelX();
		float subY = CameraGetSubPixelY();
		vec3 negCameraPos = {-cx + subX, -cy + subY, 0.0f};
		glm_translate(view, negCameraPos);
	} else {
		float subX = CameraGetSubPixelX();
		float subY = CameraGetSubPixelY();
		vec3 subPixelCompensation = {subX, subY, 0.0f};
		glm_translate(view, subPixelCompensation);
	}

	ShaderSetUniformMatrix4(shader, "projection", projectionMatrix, false);
	ShaderSetUniformMatrix4(shader, "model", model, false);
	ShaderSetUniformMatrix4(shader, "view", view, false);
	ShaderSetUniformVector4fV(shader, "color", colorV, false);

	glBindVertexArray(vao);

	// Draw as filled quad (same VAO as your rectangle)
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);
}

void DrawRectImpl(RectangleF* rect, Color* color, int filled, int useCamera) {
	Shader* shader = GetDefaultRectShader();
	ShaderUse(shader);
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){floorf(rect->x), floorf(rect->y), 0.0f});
	glm_scale(model, (vec3){rect->w, rect->h, 1.0f});
	vec4 colorV = {color->R / (float)255, color->G / (float)255, color->B / (float)255, color->A / (float)255};
	mat4 view;
	glm_mat4_identity(view);
	if (useCamera) {
		float cx = CameraGetX();
		float cy = CameraGetY();
		float subX = CameraGetSubPixelX();
		float subY = CameraGetSubPixelY();
		vec3 negCameraPos = {-cx + subX, -cy + subY, 0.0f};
		glm_translate(view, negCameraPos);
	} else {
		float subX = CameraGetSubPixelX();
		float subY = CameraGetSubPixelY();
		vec3 subPixelCompensation = {subX, subY, 0.0f};
		glm_translate(view, subPixelCompensation);
	}

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
}

void GraphicsUpdateFBOColorImpl(Color* color) {
	_fboColor = *color;
}
Color GraphicsGetFBOColorImpl(void) {
	return _fboColor;
}

int GraphicsGetTargetRefreshRateImpl(void) {
	return _refreshRate;
}

void* GraphicsGetContextPtrImpl(void) {
	return _context;
}
