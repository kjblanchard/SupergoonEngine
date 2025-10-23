#include <glad/glad.h>
// must be forst
#include <SDL3/SDL_opengl.h>
// #include <SDL3/SDL_surface.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Platform/opengl/openglGraphics.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <cglm/cglm.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_CACHED_TEXTURES 64
typedef struct Texture {
	unsigned int ID;
	unsigned int Width;
	unsigned int Height;
	unsigned int VAO;
	char* Name;
} Texture;

void TextureBindImpl(Texture* texture) {
	glBindTexture(GL_TEXTURE_2D, texture->ID);
}

// Texture _cachedTextures[MAX_CACHED_TEXTURES];

// static Texture* getTextureFromCache(const char* filename) {
// }

Texture* TextureCreateImpl(void) {
	Texture* texture = malloc(sizeof(Texture));
	texture->ID = 0;
	texture->Width = 0;
	texture->Height = 0;
	texture->Name = NULL;
	// configure VAO/VBO
	unsigned int VBO;
	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f};

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
	return texture;
}

int TextureGetWidthImpl(Texture* texture) {
	return texture->Width;
}
int TextureGetHeightImpl(Texture* texture) {
	return texture->Height;
}
void TextureLoadFromBmpImpl(Texture* texture, const char* filepath) {
	char* fullFilepath;
	asprintf(&fullFilepath, "%sassets/img/%s.bmp", GetBasePath(), filepath);
	// load image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(fullFilepath, &width, &height, &nrChannels, 0);

	texture->Name = strdup(filepath);
	texture->Width = width;
	texture->Height = height;
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// avoid row alignment issues
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->Width, texture->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		sgLogError("GL error after glTexImage2D: 0x%X", err);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	free(fullFilepath);
}
// void TextureLoadFromBmpImpl(Texture* texture, const char* filepath) {
// 	char* fullFilepath;
// 	asprintf(&fullFilepath, "%sassets/img/%s.bmp", GetBasePath(), filepath);
// 	SDL_Surface* surface = SDL_LoadBMP(fullFilepath);
// 	if (!surface) {
// 		sgLogError("Could not load bmp into surface, %s %s", fullFilepath, SDL_GetError());
// 		goto cleanup;
// 	}
// 	SDL_Color transparentColor = {255, 0, 255, 255};
// 	if (!SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, transparentColor.r, transparentColor.g, transparentColor.b))) {
// 		sgLogWarn("Failed to set color key: %s", SDL_GetError());
// 		goto cleanup;
// 	}

// 	SDL_Surface* converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
// 	if (!converted) {
// 		sgLogWarn("Failed to convert, %s", SDL_GetError());
// 		goto cleanup;
// 	}
// 	surface = converted;

// 	texture->Name = strdup(filepath);
// 	texture->Width = surface->w;
// 	texture->Height = surface->h;
// 	glBindTexture(GL_TEXTURE_2D, texture->ID);
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// avoid row alignment issues
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->Width, texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
// 	GLenum err = glGetError();
// 	if (err != GL_NO_ERROR) {
// 		sgLogError("GL error after glTexImage2D: 0x%X", err);
// 	}
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glBindTexture(GL_TEXTURE_2D, 0);
// 	SDL_DestroySurface(converted);
// cleanup:
// 	free(fullFilepath);
// 	SDL_DestroySurface(surface);
// }

void DrawTextureImpl(Texture* texture, Shader* shader, int x, int y) {
	// prepare transformations
	ShaderUse(shader);
	mat4 model;
	glm_mat4_identity(model);
	vec3 pos = {10, 10, 0};
	glm_translate(model, pos);
	vec3 size = {64, 64, 1.0f};
	glm_scale(model, size);
	ShaderSetUniformMatrix4(shader, "model", model, false);
	ShaderSetUniformMatrix4(shader, "projection", projectionMatrix, false);
	ShaderSetUniformInteger(shader, "image", 0, false);
	vec3 color = {1.0f, 1.0f, 1.0f};
	ShaderSetUniformVector3fV(shader, "spriteColor", color, false);
	glActiveTexture(GL_TEXTURE0);
	TextureBindImpl(texture);
	glBindVertexArray(texture->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void TextureDestroyImpl(Texture* texture) {
	free(texture);
}
