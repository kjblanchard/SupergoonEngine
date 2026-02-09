#include <Supergoon/Graphics/texture.h>

extern Texture* TextureCreateImpl(const char* name);
extern int TextureGetWidthImpl(Texture* texture);
extern int TextureGetHeightImpl(Texture* texture);
extern void DrawTextureImpl(Texture* texture, Shader* shader, RectangleF* dst,
							RectangleF* src, bool camera, float scale, bool flipY, Color* color);
extern void TextureLoadFromPngImpl(Texture* texture, const char* filepath);
extern void TextureDestroyImpl(Texture* texture);
extern void TextureBindImpl(Texture* texture);
extern Texture* TextureCreateRenderTargetImpl(int width, int height);
extern void SetRenderTargetImpl(Texture* renderTarget);
extern void TextureClearRenderTargetImpl(Texture* texture, float r, float g,
										 float b, float a);
extern Texture* TextureCreateNoCacheImpl(void);
extern void DrawTextureToTextureImpl(Texture* dstTarget, Texture* srcTexture,
									 Shader* shader, RectangleF* dstRect,
									 RectangleF* srcRect, float scale);
extern void SetPreviousRenderTargetImpl(void);
extern void TextureLoadFromDataImpl(Texture* texture, const char* name, int width, int height, void* data);
extern Texture** GetCachedTexturesImpl(void);
extern int GetNumCachedTexturesImpl(void);

Texture* TextureCreateNoCache(void) { return TextureCreateNoCacheImpl(); }
Texture* TextureCreate(const char* name) { return TextureCreateImpl(name); }
void TextureLoadFromPng(Texture* texture, const char* filepath) {
	TextureLoadFromPngImpl(texture, filepath);
}
int TextureGetWidth(Texture* texture) { return TextureGetWidthImpl(texture); }
int TextureGetHeight(Texture* texture) { return TextureGetHeightImpl(texture); }
void DrawTexture(Texture* texture, Shader* shader, RectangleF* dst,
				 RectangleF* src, int camera, float scale, int flipY, Color* color) {
	DrawTextureImpl(texture, shader, dst, src, camera, scale, flipY, color);
}
void TextureDestroy(Texture* texture) { TextureDestroyImpl(texture); }
void TextureBind(Texture* texture) { TextureBindImpl(texture); }
Texture* TmeextureCreate(void);
Texture* TextureCreateRenderTarget(int width, int height) {
	return TextureCreateRenderTargetImpl(width, height);
}
void SetRenderTarget(Texture* renderTarget) {
	SetRenderTargetImpl(renderTarget);
}
void TextureClearRenderTarget(Texture* texture, float r, float g, float b,
							  float a) {
	TextureClearRenderTargetImpl(texture, r, g, b, a);
}
void DrawTextureToTexture(Texture* dstTarget, Texture* srcTexture,
						  Shader* shader, RectangleF* dstRect,
						  RectangleF* srcRect, float scale) {
	DrawTextureToTextureImpl(dstTarget, srcTexture, shader, dstRect, srcRect,
							 scale);
}
void SetPreviousRenderTarget(void) {
	SetPreviousRenderTargetImpl();
}
void TextureLoadFromData(Texture* texture, const char* name, int width, int height, void* data) {
	TextureLoadFromDataImpl(texture, name, width, height, data);
}
Texture** GetCachedTextures(void) { return GetCachedTexturesImpl(); }
int GetNumCachedTextures(void) { return GetNumCachedTexturesImpl(); }
