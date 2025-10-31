#include <Supergoon/Graphics/graphics.h>

extern void InitializeGraphicsSystemImpl(void);
extern void ShutdownGraphicsSystemImpl(void);
extern void DrawEndImpl(void);
extern void DrawStartImpl(void);
extern void GraphicsWindowResizeEventImpl(int width, int height);

void DrawRect(RectangleF *rect, sgColor *color, int filled) {}

void ClearRenderTargetTexture(Texture *texture, sgColor *color) {}

void SetTextureAlpha(Texture *texture, int newAlpha) {}

void UnloadTexture(Texture *texture) {}
void UnloadUnusedTextures(void) {}
void UnloadAllTextures(void) {}

void InitializeGraphicsSystem(void) { InitializeGraphicsSystemImpl(); }
// Texture* loadTextureFromSurface(struct SDL_Surface* surface) {
// 	return NULL;
// }
void ShutdownGraphicsSystem(void) { ShutdownGraphicsSystemImpl(); }

void DrawStart(void) { DrawStartImpl(); }
void DrawEnd(void) { DrawEndImpl(); }
void GraphicsWindowResizeEvent(int width, int height) {
  GraphicsWindowResizeEventImpl(width, height);
}
