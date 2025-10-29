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
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_CACHED_TEXTURES 64

// Global or static current render target
static Texture *g_CurrentRenderTarget = NULL;

typedef struct Texture {
  unsigned int ID;
  unsigned int Width;
  unsigned int Height;
  unsigned int VAO;
  unsigned int FBO;
  char *Name;
} Texture;

void TextureBindImpl(Texture *texture) {
  glBindTexture(GL_TEXTURE_2D, texture->ID);
}

// Texture _cachedTextures[MAX_CACHED_TEXTURES];
// static Texture* getTextureFromCache(const char* filename) {
// }

void TextureClearRenderTargetImpl(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

Texture *TextureCreateImpl(void) {
  Texture *texture = malloc(sizeof(Texture));
  texture->ID = 0;
  texture->Width = 0;
  texture->Height = 0;
  texture->FBO = 0;
  texture->Name = NULL;
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
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glGenTextures(1, &texture->ID);
  return texture;
}
Texture *TextureCreateRenderTargetImpl(int width, int height) {
  Texture *texture = malloc(sizeof(Texture));
  if (!texture)
    return NULL;
  texture->ID = 0;
  texture->Width = width;
  texture->Height = height;
  texture->VAO = 0;
  texture->FBO = 0;
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
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

  // unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return texture;
}

int TextureGetWidthImpl(Texture *texture) { return texture->Width; }
int TextureGetHeightImpl(Texture *texture) { return texture->Height; }

void TextureLoadFromBmpImpl(Texture *texture, const char *filepath) {
  char *fullFilepath;
  asprintf(&fullFilepath, "%sassets/img/%s.bmp", GetBasePath(), filepath);
  SDL_Surface *surface = SDL_LoadBMP(fullFilepath);
  if (!surface) {
    sgLogError("Could not load bmp into surface, %s %s", fullFilepath,
               SDL_GetError());
    goto cleanup;
  }
  SDL_Color transparentColor = {255, 0, 255, 255};
  if (!SDL_SetSurfaceColorKey(surface, true,
                              SDL_MapSurfaceRGB(surface, transparentColor.r,
                                                transparentColor.g,
                                                transparentColor.b))) {
    sgLogWarn("Failed to set color key: %s", SDL_GetError());
    goto cleanup;
  }
  SDL_Surface *converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
  if (!converted) {
    sgLogWarn("Failed to convert, %s", SDL_GetError());
    goto cleanup;
  }
  SDL_DestroySurface(surface);
  surface = converted;
  texture->Name = strdup(filepath);
  texture->Width = surface->w;
  texture->Height = surface->h;
  glBindTexture(GL_TEXTURE_2D, texture->ID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // avoid row alignment issues
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

void DrawTextureImpl(Texture *texture, Shader *shader, RectangleF *dstRect,
                     RectangleF *srcRect, float scale) {
  // prepare transformations
  ShaderUse(shader);
  mat4 model;
  glm_mat4_identity(model);
  vec3 pos = {round(dstRect->x), round(dstRect->y), 0};
  glm_translate(model, pos);
  vec3 size = {dstRect->w * scale, dstRect->h * scale, 1.0f};
  glm_scale(model, size);
  static vec3 cameraPos = {0.0f, 0.0f, 0.0f};
  mat4 view;
  glm_mat4_identity(view);
  vec3 negCameraPos = {-cameraPos[0], -cameraPos[1], 0.0f};
  glm_translate(view, negCameraPos);

  vec4 srcRectV = {round(srcRect->x), srcRect->y, srcRect->w, srcRect->h};
  vec2 texSize = {(float)texture->Width, (float)texture->Height};
  ShaderSetUniformVector4fV(shader, "srcRect", srcRectV, false);
  ShaderSetUniformVector2fV(shader, "textureSize", texSize, false);

  // compute UVs on CPU test
  // float u0 = srcRect->x / texture->Width;
  // float v0 = srcRect->y / texture->Height;
  // float u1 = (srcRect->x + srcRect->w) / texture->Width;
  // float v1 = (srcRect->y + srcRect->h) / texture->Height;
  // ShaderSetUniformVector4f(shader, "uvRect", u0, v0, u1, v1, false);

  ShaderSetUniformMatrix4(shader, "model", model, false);
  ShaderSetUniformMatrix4(shader, "view", view, false);
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

void TextureDestroyImpl(Texture *texture) { free(texture); }

void SetRenderTargetImpl(Texture *target) {
  if (target) {
    glBindFramebuffer(GL_FRAMEBUFFER, target->FBO);
    glViewport(0, 0, target->Width, target->Height);
    g_CurrentRenderTarget = target;
  } else {
    // Restore to system default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WindowWidth(), WindowHeight());
    g_CurrentRenderTarget = NULL;
  }
}
