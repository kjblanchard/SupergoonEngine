#include <Supergoon/Graphics/texture.h>

extern Texture* TextureCreateImpl(void);
extern int TextureGetWidthImpl(Texture* texture);
extern int TextureGetHeightImpl(Texture* texture);
extern void DrawTextureImpl(Texture* texture, Shader* shader, float x, float y);
extern void TextureLoadFromBmpImpl(Texture* texture, const char* filepath);
extern void TextureDestroyImpl(Texture* texture);
extern void TextureBindImpl(Texture* texture);

Texture* TextureCreate(void) { return TextureCreateImpl(); }
void TextureLoadFromBmp(Texture* texture, const char* filepath) { TextureLoadFromBmpImpl(texture, filepath); }
int TextureGetWidth(Texture* texture) { return TextureGetWidthImpl(texture); }
int TextureGetHeight(Texture* texture) { return TextureGetHeightImpl(texture); }
void DrawTexture(Texture* texture, Shader* shader, float x, float y) { DrawTextureImpl(texture, shader, x, y); }
void TextureDestroy(Texture* texture) { TextureDestroyImpl(texture); }
void TextureBind(Texture* texture) { TextureBindImpl(texture); }
