#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Platform/opengl/openglTexture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/camera.h>
#include <sgtools/tools.h>
#include <stdbool.h>
#include <string.h>
#if !defined(__EMSCRIPTEN__) && !defined(ANDROID) && !defined(USE_GLES)
#include <glad/glad.h>
// Need to do glad first
#include <SDL3/SDL_opengl.h>
#else
#ifdef __APPLE__
#include <OpenGLES/ES3/gl.h>
#else
#include <GLES3/gl3.h>
#endif
#include <SDL3/SDL_opengles2.h>
#endif
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_surface.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Platform/opengl/openglGraphics.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/window.h>
#include <cglm/cglm.h>
#include <sgtools/log.h>
#include <stdio.h>
#include <stdlib.h>

static inline void colorToVec4(const Color* c, vec4 out) {
	out[0] = (float)c->R / 255;
	out[1] = (float)c->G / 255;
	out[2] = (float)c->B / 255;
	out[3] = (float)c->A / 255;
}

static void buildViewMatrix(mat4 view, int useCamera) {
	glm_mat4_identity(view);
	if (useCamera) {
		vec3 negCameraPos = {-CameraGetX(), -CameraGetY(), 0.0f};
		glm_translate(view, negCameraPos);
	}
}

static void setupTextureQuadVAO(Texture* texture) {
	static float vertices[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f};
	glGenVertexArrays(1, &texture->VAO);
	glGenBuffers(1, &texture->VBO);
	glBindVertexArray(texture->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, texture->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// TODO get rid of this
extern GLint defaultFBO;
static Texture* currentRenderingTarget = NULL;
static int currentRenderingTargetWidth = 0;
static int currentRenderingTargetHeight = 0;
static Texture* previousRenderingTarget = NULL;

void TextureBindImpl(Texture* texture) {
	glBindTexture(GL_TEXTURE_2D, texture->ID);
}

void TextureClearRenderTargetImpl(Texture* texture, float r, float g, float b, float a) {
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
	texture->VBO = 0;
	texture->FBO = 0;
	texture->Name = NULL;
	setupTextureQuadVAO(texture);
	glGenTextures(1, &texture->ID);
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return texture;
}

void* TextureGetIDImpl(Texture* texture) {
	return (void*)(intptr_t)texture->ID;
}

Texture* TextureCreateImpl(const char* name) {
	sgLogDebug("Loading new texture: %s", name);
	Texture* texture = TextureCreateNoCacheImpl();
	texture->Name = strdup(name);
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
	texture->VBO = 0;
	texture->FBO = 0;
	asprintf(&texture->Name, "%d_%d_render_target_framebuffer", width, height);
#if !defined(__EMSCRIPTEN__) && !defined(ANDROID) && !defined(USE_GLES)
	GLint internalFormat = GL_RGBA8;
#else
	GLint internalFormat = GL_RGBA;
#endif
	glGenTextures(1, &texture->ID);
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Create framebuffer and attach texture as color attachment 0
	glGenFramebuffers(1, &texture->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, texture->FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->ID, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		sgLogError("Framebuffer not complete for render target %s (status 0x%X)",
				   texture->Name, status);
	} else {
		sgLogDebug("[FBO] Created render target %dx%d FBO=%u tex=%u status=COMPLETE",
				   width, height, texture->FBO, texture->ID);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	setupTextureQuadVAO(texture);
	return texture;
}

int TextureGetWidthImpl(Texture* texture) { return texture->Width; }
int TextureGetHeightImpl(Texture* texture) { return texture->Height; }

static void uploadRGBAPixels(Texture* texture, int w, int h, void* pixels) {
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		sgLogError("GL error after glTexImage2D: 0x%X", err);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureLoadFromPngImpl(Texture* texture, const char* filepath) {
	if (texture->Width || texture->Height) return;
	sgLogDebug("Loading from png %s", filepath);
	char* fullFilepath;
	asprintf(&fullFilepath, "%sassets/img/%s.png", GetBasePath(), filepath);
	SDL_Surface* surface = SDL_LoadPNG(fullFilepath);
	if (!surface) {
		sgLogError("Could not load png into surface, %s %s", fullFilepath,
				   SDL_GetError());
		goto cleanup;
	}
	texture->Width = surface->w;
	texture->Height = surface->h;
	uploadRGBAPixels(texture, surface->w, surface->h, surface->pixels);
cleanup:
	free(fullFilepath);
	SDL_DestroySurface(surface);
}

void TextureLoadFromPngBufferImpl(Texture* texture, const char* filepath, char* buf, size_t sz) {
	if (texture->Width || texture->Height) return;
	sgLogDebug("Loading from png buffer %s", filepath);
	SDL_IOStream* stream = SDL_IOFromMem(buf, sz);
	if (!stream) {
		sgLogError("Could not load stream for image, %s %s", filepath, SDL_GetError());
		return;
	}
	SDL_Surface* surface = SDL_LoadPNG_IO(stream, true);
	if (!surface) {
		sgLogError("Could not load png into surface, %s %s", filepath, SDL_GetError());
		SDL_CloseIO(stream);
		goto cleanup;
	}
	texture->Width = surface->w;
	texture->Height = surface->h;
	uploadRGBAPixels(texture, surface->w, surface->h, surface->pixels);
cleanup:
	SDL_DestroySurface(surface);
}

void DrawTextureRaw(Texture* texture, Shader* shader, RectangleF* dstRect, RectangleF* srcRect, bool useCamera, float scale, bool flipY, Color* color) {
	if (flipY) {
		dstRect->y += dstRect->h * scale;
		dstRect->h *= -1;
	}
	ShaderUse(shader);
	mat4 model;
	glm_mat4_identity(model);
	vec3 pos = {floorf(dstRect->x), floorf(dstRect->y), 0};
	glm_translate(model, pos);
	vec3 size = {dstRect->w * scale, dstRect->h * scale, 1.0f};
	glm_scale(model, size);
	mat4 view;
	buildViewMatrix(view, useCamera);
	vec4 srcRectV = {floorf(srcRect->x), floorf(srcRect->y), srcRect->w, srcRect->h};
	vec2 texSize = {(float)texture->Width, (float)texture->Height};
	ShaderSetUniformVector4fV(shader, "srcRect", srcRectV, false);
	ShaderSetUniformVector2fV(shader, "textureSize", texSize, false);
	ShaderSetUniformMatrix4(shader, "model", model, false);
	ShaderSetUniformMatrix4(shader, "view", view, false);
	ShaderSetUniformMatrix4(shader, "projection", projectionMatrix, false);
	ShaderSetUniformInteger(shader, "image", 0, false);
	vec4 colorVec;
	colorToVec4(color, colorVec);
	ShaderSetUniformVector4fV(shader, "spriteColor", colorVec, false);
	glActiveTexture(GL_TEXTURE0);
	TextureBindImpl(texture);
	glBindVertexArray(texture->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void DrawTextureToScreenImpl(Texture* texture, Shader* shader, RectangleF* dstRect, bool flipY, Color* color) {
	if (flipY) {
		dstRect->y += dstRect->h;
		dstRect->h *= -1;
	}
	ShaderUse(shader);
	mat4 model;
	glm_mat4_identity(model);
	vec3 pos = {dstRect->x, dstRect->y, 0};
	glm_translate(model, pos);
	vec3 size = {dstRect->w, dstRect->h, 1.0f};
	glm_scale(model, size);
	ShaderSetUniformMatrix4(shader, "model", model, false);
	ShaderSetUniformMatrix4(shader, "projection", projectionMatrix, false);
	ShaderSetUniformInteger(shader, "image", 0, false);
	vec4 colorVec;
	colorToVec4(color, colorVec);
	ShaderSetUniformVector4fV(shader, "spriteColor", colorVec, false);
	glActiveTexture(GL_TEXTURE0);
	TextureBindImpl(texture);
	glBindVertexArray(texture->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void DrawTextureImpl(Texture* texture, Shader* shader, RectangleF* dstRect, RectangleF* srcRect, bool useCamera, float scale, bool flipY, Color* color) {
	DrawTextureRaw(texture, shader, dstRect, srcRect, useCamera, scale, flipY, color);
}

void DrawTextureToTextureImpl(Texture* dstTarget, Texture* srcTexture, Shader* shader, RectangleF* dstRect, RectangleF* srcRect, float scale) {
	SetRenderTarget(dstTarget);
	Color color = {255, 255, 255, 255};
	DrawTexture(srcTexture, shader, dstRect, srcRect, false, scale, false, &color);
	SetPreviousRenderTarget();
}

void TextureDestroyImpl(Texture* texture) {
	return;
	if (!texture) return;
	if (texture->ID != 0) {
		glDeleteTextures(1, &texture->ID);
		texture->ID = 0;
	}
	if (texture->FBO != 0) {
		glDeleteFramebuffers(1, &texture->FBO);
		texture->FBO = 0;
	}
	if (texture->VBO != 0) {
		glDeleteBuffers(1, &texture->VBO);
		texture->VBO = 0;
	}
	if (texture->VAO != 0) {
		glDeleteVertexArrays(1, &texture->VAO);
		texture->VAO = 0;
	}
	if (texture->Name) {
		free(texture->Name);
	}
	texture->Width = 0;
	texture->Height = 0;
	free(texture);
}

void SetPreviousRenderTargetImpl(void) {
	SetRenderTarget(previousRenderingTarget);
}

static int renderTargetLogCount = 0;
void SetRenderTargetImpl(Texture* target) {
	previousRenderingTarget = currentRenderingTarget;
	if (target) {
		currentRenderingTargetWidth = target->Width;
		currentRenderingTargetHeight = target->Height;
		glBindFramebuffer(GL_FRAMEBUFFER, target->FBO);
	} else {
		currentRenderingTargetWidth = WindowWidth();
		currentRenderingTargetHeight = WindowHeight();
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
		if (renderTargetLogCount < 5) {
			sgLogDebug("[RT] Binding default FBO: %d (win %dx%d)", defaultFBO, currentRenderingTargetWidth, currentRenderingTargetHeight);
			++renderTargetLogCount;
		}
	}
	currentRenderingTarget = target;
	glm_ortho(0.0f, currentRenderingTargetWidth, 0.0f, currentRenderingTargetHeight, -1.0f, 1.0f, projectionMatrix);
	glViewport(0, 0, currentRenderingTargetWidth, currentRenderingTargetHeight);
}

void TextureLoadFromDataImpl(Texture* texture, const char* name, int width, int height, void* data) {
	texture->Name = strdup(name);
	texture->Width = width;
	texture->Height = height;
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texture->Width, texture->Height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}
