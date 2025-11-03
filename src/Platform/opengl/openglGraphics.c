#include <SDL3/SDL_video.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/camera.h>
#include <Supergoon/window.h>
#include <stdlib.h>
#include <string.h>
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
#include <Supergoon/log.h>

extern void ShaderSystemShutdown(void);
SDL_GLContext _context;
static Texture* _screenFrameBufferTexture = NULL;
static int _logicalX = 0;
static int _logicalY = 0;
// TODO for now, only use the refresh rate set here.. we should set it eventually.
static unsigned int _refreshRate = 999;
#ifndef __EMSCRIPTEN__
static bool _vsync = true;
#endif

mat4 projectionMatrix;
void GraphicsWindowResizeEventImpl(int width, int height) {
	if (!_context) {
		return;
	}
	glViewport(0, 0, width, height);
	glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, projectionMatrix);
	int texWidth = width;
	int texHeight = height;
	if (_logicalX && _logicalY) {
		texWidth = _logicalX;
		texHeight = _logicalY;
	}
	SetCameraSize(texWidth, texHeight);
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
	sgLogWarn("OpenGL version: %s", glGetString(GL_VERSION));
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

	int fbWidth = TextureGetWidth(_screenFrameBufferTexture);
	int fbHeight = TextureGetHeight(_screenFrameBufferTexture);
	int winWidth = WindowWidth();
	int winHeight = WindowHeight();

	// Compute integer scaling factor
	int scaleX = winWidth / fbWidth;
	int scaleY = winHeight / fbHeight;
	int scale = scaleX < scaleY ? scaleX : scaleY;
	if (scale < 1) scale = 1;  // don't shrink below 1x

	// Compute destination rectangle to center the framebuffer
	int drawWidth = fbWidth * scale;
	int drawHeight = fbHeight * scale;
	int offsetX = (winWidth - drawWidth) / 2;
	int offsetY = (winHeight - drawHeight) / 2;

	RectangleF dstRect = {
		(float)offsetX,
		(float)offsetY,
		(float)drawWidth,
		(float)drawHeight};

	RectangleF srcRect = {0, 0, (float)fbWidth, (float)fbHeight};
	sgColor color = {255, 255, 255, 255};
	DrawTexture(_screenFrameBufferTexture, GetDefaultShader(), &dstRect, &srcRect, false, 1.0f, true, &color);

	SDL_GL_SwapWindow(WindowGetImpl()->Handle);
}

/* void DrawEndImpl(void) { */
/* 	SetRenderTarget(NULL); */
/* 	int texX = TextureGetWidth(_screenFrameBufferTexture); */
/* 	int texY = TextureGetHeight(_screenFrameBufferTexture); */
/* 	int scaleX = texX / WindowWidth(); */
/* 	int scaleY = texY / WindowHeight(); */
/* 	DrawTexture(_screenFrameBufferTexture, GetDefaultShader(), &(RectangleF){0, 0, WindowWidth(), WindowHeight()}, &(RectangleF){0, 0, texX, texY}, false, 1.0f, true); */
/* 	SDL_GL_SwapWindow(_window); */
/* } */
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

int GraphicsGetTargetRefreshRateImpl(void) {
	return _refreshRate;
}
