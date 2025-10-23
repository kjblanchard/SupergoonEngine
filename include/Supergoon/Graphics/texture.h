#pragma once

typedef struct Texture Texture;
typedef struct Shader Shader;

Texture* TextureCreate(void);
void TextureLoadFromBmp(Texture* texture, const char* filepath);
int TextureGetWidth(Texture* texture);
int TextureGetHeight(Texture* texture);
void DrawTexture(Texture* texture, Shader* shader, float x, float y);
void TextureDestroy(Texture* texture);
void TextureBind(Texture* texture);
