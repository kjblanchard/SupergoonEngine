#include <Supergoon/Graphics/texture.h>

extern Texture* TextureCreateImpl(void);
// extern Texture* TextureCreateRenderTargetImpl(int width, int height, int r, int g, int b, int a);
extern int TextureGetWidthImpl(Texture* texture);
extern int TextureGetHeightImpl(Texture* texture);
extern void DrawTextureImpl(Texture* texture, Shader* shader, RectangleF* dst, RectangleF* src, float scale);
extern void TextureLoadFromBmpImpl(Texture* texture, const char* filepath);
extern void TextureDestroyImpl(Texture* texture);
extern void TextureBindImpl(Texture* texture);

Texture* TextureCreate(void) { return TextureCreateImpl(); }
// Texture* TextureCreateRenderTarget(int width, int height, int r, int g, int b, int a) { return TextureCreateRenderTargetImpl(width, height, r, g, b, a); }
void TextureLoadFromBmp(Texture* texture, const char* filepath) { TextureLoadFromBmpImpl(texture, filepath); }
int TextureGetWidth(Texture* texture) { return TextureGetWidthImpl(texture); }
int TextureGetHeight(Texture* texture) { return TextureGetHeightImpl(texture); }
void DrawTexture(Texture* texture, Shader* shader, RectangleF* dst, RectangleF* src, float scale) { DrawTextureImpl(texture, shader, dst, src, scale); }
void TextureDestroy(Texture* texture) { TextureDestroyImpl(texture); }
void TextureBind(Texture* texture) { TextureBindImpl(texture); }
