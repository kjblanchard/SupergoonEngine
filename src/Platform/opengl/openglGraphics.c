#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/camera.h>
#include <Supergoon/window.h>
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

mat4 projectionMatrix;
void GraphicsWindowResizeEventImpl(int width, int height) {
	if (!_context) {
		return;
	}
	glViewport(0, 0, width, height);
	glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, projectionMatrix);
	if (_screenFrameBufferTexture) {
		TextureDestroy(_screenFrameBufferTexture);
	}
	int texWidth = width;
	int texHeight = height;
	if (_logicalX && _logicalY) {
		texWidth = _logicalX;
		texHeight = _logicalY;
		glViewport(0, 0, texWidth, texHeight);
		glm_ortho(0.0f, texWidth, texHeight, 0.0f, -1.0f, 1.0f, projectionMatrix);
	}
	_screenFrameBufferTexture = TextureCreateRenderTarget(texWidth, texHeight);
	TextureClearRenderTarget(_screenFrameBufferTexture, 0, 0, 0, 1.0);
	// We need to handle if the resolution is different, with a logical size set.
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
	_context = SDL_GL_CreateContext(_window);
	if (!_context) {
		sgLogCritical("Could not create opengl context, exiting! %s",
					  SDL_GetError());
	}
	// SDL_GL_MakeCurrent(_window, _context);
#ifndef __EMSCRIPTEN__
	if (!gladLoadGL()) {
		sgLogError("Failed to initialize GLAD!");
		return;
	}
#endif
	sgLogWarn("OpenGL version: %s", glGetString(GL_VERSION));
	// Set the projection matrix to the screen size.
	int width = WindowWidthImpl();
	int height = WindowHeightImpl();
	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm_ortho(0.0f, WindowWidthImpl(), WindowHeightImpl(), 0.0f, -1.0f, 1.0f,
			  projectionMatrix);
	SDL_GL_SetSwapInterval(1);	// vsync
}

void ShutdownGraphicsSystemImpl(void) {
	ShaderSystemShutdown();
	SDL_GL_DestroyContext(_context);
}
void DrawStartImpl(void) {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SetRenderTarget(_screenFrameBufferTexture);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void DrawEndImpl(void) {
	SetRenderTarget(NULL);
	int texX = TextureGetWidth(_screenFrameBufferTexture);
	int texY = TextureGetHeight(_screenFrameBufferTexture);
	int scaleX = 1.0f;

	/* if (_logicalX > 0 && _logicalY > 0) { */
	/* 	scaleX = (float)WindowWidth() / (float)_logicalX; */
	/* 	/1* scaleY = (float)WindowHeight() / (float)_logicalY; *1/ */
	/* } */
	DrawTexture(_screenFrameBufferTexture, GetDefaultShader(), &(RectangleF){0, 0, WindowWidth(), WindowHeight()}, &(RectangleF){0, 0, texX, texY}, false, scaleX, true);
	SDL_GL_SwapWindow(_window);
}
void GraphicsSetLogicalWorldSizeImpl(int width, int height) {
	_logicalX = width;
	_logicalY = height;
}
