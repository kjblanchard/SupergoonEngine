#include <glad/glad.h>
// Need to do glad first
#include <SDL3/SDL_opengl.h>
#include <Supergoon/Platform/opengl/openglGraphics.h>
#include <Supergoon/Platform/sdl/sdlWindow.h>
#include <Supergoon/log.h>

extern void ShaderSystemShutdown(void);
SDL_GLContext _context;

mat4 projectionMatrix;
void GraphicsWindowResizeEventImpl(int width, int height) {
	if (!_context) {
		return;
	}
	glViewport(0, 0, width, height);
	glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, projectionMatrix);
}

void InitializeGraphicsSystemImpl(void) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	_context = SDL_GL_CreateContext(_window);
	if (!_context) {
		sgLogCritical("Could not create opengl context, exiting! %s", SDL_GetError());
	}
	SDL_GL_MakeCurrent(_window, _context);
	if (!gladLoadGL()) {
		sgLogError("Failed to initialize GLAD!");
		return;
	}
	sgLogDebug("OpenGL version: %s", glGetString(GL_VERSION));
	// Set the projection matrix to the screen size.
	int width = WindowWidthImpl();
	int height = WindowHeightImpl();
	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm_ortho(0.0f, WindowWidthImpl(), WindowHeightImpl(), 0.0f, -1.0f, 1.0f, projectionMatrix);
	SDL_GL_SetSwapInterval(1);	// vsync
}

void ShutdownGraphicsSystemImpl(void) {
	ShaderSystemShutdown();
	SDL_GL_DestroyContext(_context);
}
void DrawStartImpl(void) {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void DrawEndImpl(void) {
	SDL_GL_SwapWindow(_window);
}
