#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/camera.h>
#include <Supergoon/tools.h>
#include <stdbool.h>
#include <string.h>
#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
// must be forst<SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl.h>
#else
#include <GLES3/gl3.h>
#include <SDL3/SDL_opengles2.h>
#endif
#include <SDL3/SDL_surface.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Platform/opengl/openglGraphics.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_CACHED_TEXTURES 64

static Texture* _currentRenderingTarget = NULL;
static int _currentRenderingTargetWidth = 0;
static int _currentRenderingTargetHeight = 0;
static Texture* _previousRenderingTarget = NULL;
static int _currentCachedTextures = 0;
Texture* _cachedTextures[MAX_CACHED_TEXTURES];

typedef struct Texture {
	unsigned int ID;
	unsigned int Width;
	unsigned int Height;
	unsigned int VAO;
	unsigned int FBO;
	int RefCount;
	char* Name;
} Texture;

void TextureBindImpl(Texture* texture) {
	glBindTexture(GL_TEXTURE_2D, texture->ID);
}

static Texture* getTextureFromCache(const char* filename) {
	Texture* returnTexture = NULL;
	for (int i = 0; i < _currentCachedTextures; ++i) {
		if (strcmp(filename, _cachedTextures[i]->Name) == 0) {
			returnTexture = _cachedTextures[i];
			break;
		}
	}
	return returnTexture;
}

static void cacheTexture(Texture* texture) {
	for (int i = 0; i < _currentCachedTextures; ++i) {
		if (!_cachedTextures[i]) {
			_cachedTextures[i] = texture;
			return;
		}
	}
}

void TextureClearRenderTargetImpl(Texture* texture, float r, float g, float b,
								  float a) {
	SetRenderTarget(texture);
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
	SetPreviousRenderTarget();
}
Texture* TextureCreateNoCacheImpl(void) {
	Texture* texture = malloc(sizeof(Texture));
	texture->ID = 0;
	texture->Width = 0;
	texture->Height = 0;
	texture->FBO = 0;
	texture->Name = NULL;
	texture->RefCount = 1;
	// configure VAO/VBO
	unsigned int VBO;
	float vertices[] = {// pos      // tex
						0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
						1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
	glGenVertexArrays(1, &texture->VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(texture->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glGenTextures(1, &texture->ID);

	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return texture;
}

Texture* TextureCreateImpl(const char* name) {
	Texture* texture = getTextureFromCache(name);
	if (texture) {
		++texture->RefCount;
		return texture;
	}
	texture = TextureCreateNoCacheImpl();
	cacheTexture(texture);
	return texture;
}
Texture* TextureCreateRenderTargetImpl(int width, int height) {
	Texture* texture = malloc(sizeof(Texture));
	if (!texture)
		return NULL;
	texture->ID = 0;
	texture->Width = width;
	texture->Height = height;
	texture->VAO = 0;
	texture->FBO = 0;
	texture->RefCount = 1;
	asprintf(&texture->Name, "%d_%d_render_target_framebuffer", width, height);
	// Create GL texture
#ifndef __EMSCRIPTEN__
	GLint internalFormat = GL_RGBA8;
#else
	GLint internalFormat = GL_RGBA;
#endif
	glGenTextures(1, &texture->ID);
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, NULL);

	// sensible defaults for render target texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create framebuffer and attach texture as color attachment 0
	glGenFramebuffers(1, &texture->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, texture->FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   texture->ID, 0);

	// (Optional) If you need depth/stencil, create a renderbuffer here.
	// For now we assume color-only render target.

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		sgLogError("Framebuffer not complete for render target %s (status 0x%X)",
				   texture->Name, status);
		// continue — but you may want to fail/cleanup here
	}

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setup a simple quad VAO (positions + texcoords packed in vec4 like your
	// other code)
	unsigned int VBO = 0;
	float vertices[] = {// pos(x,y)   // tex(u,v)
						0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
						1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

	glGenVertexArrays(1, &texture->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(texture->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	// attribute 0 is vec4 (pos.xy, tex.xy) like your current code
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return texture;
}

int TextureGetWidthImpl(Texture* texture) { return texture->Width; }
int TextureGetHeightImpl(Texture* texture) { return texture->Height; }

void TextureLoadFromPngImpl(Texture* texture, const char* filepath) {
	char* fullFilepath;
	asprintf(&fullFilepath, "%sassets/img/%s.png", GetBasePath(), filepath);
	SDL_Surface* surface = SDL_LoadPNG(fullFilepath);
	if (!surface) {
		sgLogError("Could not load png into surface, %s %s", fullFilepath,
				   SDL_GetError());
		goto cleanup;
	}
	texture->Name = strdup(filepath);
	texture->Width = surface->w;
	texture->Height = surface->h;
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// avoid row alignment issues
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->Width, texture->Height, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		sgLogError("GL error after glTexImage2D: 0x%X", err);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
cleanup:
	free(fullFilepath);
	SDL_DestroySurface(surface);
}

void DrawTextureImpl(Texture* texture, Shader* shader, RectangleF* dstRect,
					 RectangleF* srcRect, bool useCamera, float scale, bool flipY, Color* color) {
	if (flipY) {
		dstRect->y += dstRect->h * scale;  // move origin to top
		dstRect->h *= -1;				   // negative height flips it
	}
	ShaderUse(shader);
	mat4 model;
	glm_mat4_identity(model);
	/* vec3 pos = {floorf(dstRect->x), floorf(dstRect->y), 0}; */
	vec3 pos = {(dstRect->x), (dstRect->y), 0};
	glm_translate(model, pos);
	vec3 size = {dstRect->w * scale, dstRect->h * scale, 1.0f};
	glm_scale(model, size);
	mat4 view;
	glm_mat4_identity(view);
	if (useCamera) {
		double cx = CameraGetX();
		double cy = CameraGetY();
		vec3 negCameraPos = {
			-(cx),
			-(cy),
			0.0f};

		glm_translate(view, negCameraPos);
	}
	vec4 srcRectV = {floorf(srcRect->x), floorf(srcRect->y), srcRect->w, srcRect->h};
	vec2 texSize = {(float)texture->Width, (float)texture->Height};
	ShaderSetUniformVector4fV(shader, "srcRect", srcRectV, false);
	ShaderSetUniformVector2fV(shader, "textureSize", texSize, false);
	ShaderSetUniformMatrix4(shader, "model", model, false);
	ShaderSetUniformMatrix4(shader, "view", view, false);
	ShaderSetUniformMatrix4(shader, "projection", projectionMatrix, false);
	ShaderSetUniformInteger(shader, "image", 0, false);
	/* vec3 colorVec = {color->R / (float)255, color->G / (float)255, color->B / (float)255}; */
	vec4 colorVec = {color->R / (float)255, color->G / (float)255, color->B / (float)255, color->A / (float)255};
	/* ShaderSetUniformVector3fV(shader, "spriteColor", colorVec, false); */
	ShaderSetUniformVector4fV(shader, "spriteColor", colorVec, false);
	glActiveTexture(GL_TEXTURE0);
	TextureBindImpl(texture);
	glBindVertexArray(texture->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void DrawTextureToTextureImpl(Texture* dstTarget, Texture* srcTexture,
							  Shader* shader, RectangleF* dstRect,
							  RectangleF* srcRect, float scale) {
	SetRenderTarget(dstTarget);
	Color color = {255, 255, 255, 255};
	DrawTexture(srcTexture, shader, dstRect, srcRect, false, scale, false, &color);
	SetPreviousRenderTarget();
}

void TextureDestroyImpl(Texture* texture) {
	if (!texture) return;
	--texture->RefCount;
	if (texture->RefCount > 0) return;

	// Delete texture object
	if (texture->ID != 0) {
		glDeleteTextures(1, &texture->ID);
		texture->ID = 0;
	}

	// Delete framebuffer if it exists
	if (texture->FBO != 0) {
		glDeleteFramebuffers(1, &texture->FBO);
		texture->FBO = 0;
	}

	// Delete VAO if it exists
	if (texture->VAO != 0) {
		glDeleteVertexArrays(1, &texture->VAO);
		texture->VAO = 0;
	}

	// Free the name string if allocated dynamically
	if (texture->Name) {
		free(texture->Name);
		texture->Name = NULL;
	}

	// Optional: clear other metadata
	texture->Width = 0;
	texture->Height = 0;
	free(texture);
}

void SetPreviousRenderTargetImpl(void) {
	SetRenderTarget(_previousRenderingTarget);
}

void SetRenderTargetImpl(Texture* target) {
	_previousRenderingTarget = _currentRenderingTarget;
	if (target) {
		_currentRenderingTargetWidth = target->Width;
		_currentRenderingTargetHeight = target->Height;
		glBindFramebuffer(GL_FRAMEBUFFER, target->FBO);
	} else {
		// Restore to system default framebuffer
		_currentRenderingTargetWidth = WindowWidth();
		_currentRenderingTargetHeight = WindowHeight();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	_currentRenderingTarget = target;
	glm_ortho(0.0f, _currentRenderingTargetWidth, 0.0f, _currentRenderingTargetHeight, -1.0f, 1.0f, projectionMatrix);
	glViewport(0, 0, _currentRenderingTargetWidth, _currentRenderingTargetHeight);
}

void TextureLoadFromDataImpl(Texture* texture, const char* name, int width, int height, void* data) {
	texture->Name = strdup(name);
	texture->Width = width;
	texture->Height = height;
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	/* glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture->Width, texture->Height, 0, */
	/* 			 GL_RED, GL_UNSIGNED_BYTE, data); */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texture->Width, texture->Height, 0,
				 GL_RED, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}
