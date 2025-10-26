#pragma once
#include <Supergoon/Primitives/rectangle.h>

typedef struct Texture Texture;
typedef struct Shader Shader;

Texture* TextureCreate(void);
Texture* TextureCreateRenderTarget(int width, int height, int r, int g, int b, int a);
void TextureLoadFromBmp(Texture* texture, const char* filepath);
int TextureGetWidth(Texture* texture);
int TextureGetHeight(Texture* texture);
void DrawTexture(Texture* texture, Shader* shader, RectangleF* dst, RectangleF* src, float scale);
void TextureDestroy(Texture* texture);
void TextureBind(Texture* texture);
