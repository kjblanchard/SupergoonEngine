#pragma once
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Texture Texture;
typedef struct Shader Shader;

//Used when you don't want the engine to cache it/draw with everything else, for text currently to keep it forever
Texture* TextureCreateNoCache(void);
// Caches the texture result, useful if you want to have the engine cleaning it up when unused
Texture* TextureCreate(const char* name);
Texture* TextureCreateRenderTarget(int width, int height);
void* TextureGetID(Texture* texture);
void SetRenderTarget(Texture* renderTarget);
//Used by debugging
int GetNumCachedTextures(void);
void SetPreviousRenderTarget(void);
void TextureLoadFromPng(Texture* texture, const char* filepath);
void TextureLoadFromPngBuffer(Texture* texture, const char* filepath, char* buf, size_t sz);
void TextureLoadFromData(Texture* texture, const char* name, int width, int height, void* data);
int TextureGetWidth(Texture* texture);
int TextureGetHeight(Texture* texture);
void TextureDestroy(Texture* texture);
void TextureBind(Texture* texture);
void TextureClearRenderTarget(Texture* texture, float r, float g, float b, float a);
//Draws to the current render target, used by most gameobjects, etc.
void DrawTexture(Texture* texture, Shader* shader, RectangleF* dst, RectangleF* src, int camera, float scale, int flipY, Color* color);
//Draws to render targets mostly.
void DrawTextureToTexture(Texture* dstTarget, Texture* srcTexture, Shader* shader, RectangleF* dstRect, RectangleF* srcRect, float scale);
//Does not draw using the camera, used for pixel perfect drawing to screen, and by the UI
void DrawTextureToScreen(Texture* texture, Shader* shader, RectangleF* dstRect, bool flipY, Color* color);
Texture** GetCachedTextures(void);
#ifdef __cplusplus
}
#endif
