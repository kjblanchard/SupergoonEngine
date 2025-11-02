#pragma once
#include <Supergoon/Primitives/rectangle.h>

typedef struct Texture Texture;
typedef struct Shader Shader;

Texture *TextureCreate(void);
Texture *TextureCreateRenderTarget(int width, int height);
void SetRenderTarget(Texture *renderTarget);
void SetPreviousRenderTarget(void);
void TextureLoadFromBmp(Texture *texture, const char *filepath);
void TextureLoadFromData(Texture *texture, const char *name, int width, int height, void *data);
int TextureGetWidth(Texture *texture);
int TextureGetHeight(Texture *texture);
void DrawTexture(Texture *texture, Shader *shader, RectangleF *dst,
				 RectangleF *src, int camera, float scale, int flipY);
void TextureDestroy(Texture *texture);
void TextureBind(Texture *texture);
void TextureClearRenderTarget(Texture *texture, float r, float g, float b,
							  float a);
void DrawTextureToTexture(Texture *dstTarget, Texture *srcTexture,
						  Shader *shader, RectangleF *dstRect,
						  RectangleF *srcRect, float scale);
