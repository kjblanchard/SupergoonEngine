#include <Supergoon/Graphics/graphics.h>

extern void InitializeGraphicsSystemImpl(void);
extern void ShutdownGraphicsSystemImpl(void);
extern void DrawEndImpl(void);
extern void DrawStartImpl(void);
extern void GraphicsWindowResizeEventImpl(int width, int height);
extern void GraphicsSetLogicalWorldSizeImpl(int width, int height);
extern int GraphicsGetTargetRefreshRateImpl(void);
extern void DrawRectImpl(RectangleF *rect, Color *color, int filled);

void DrawRect(RectangleF *rect, Color *color, int filled) {
	DrawRectImpl(rect, color, filled);
}
void ClearRenderTargetTexture(Texture *texture, Color *color) {}
void SetTextureAlpha(Texture *texture, int newAlpha) {}
void UnloadTexture(Texture *texture) {}
void UnloadUnusedTextures(void) {}
void UnloadAllTextures(void) {}
void InitializeGraphicsSystem(void) { InitializeGraphicsSystemImpl(); }
void ShutdownGraphicsSystem(void) { ShutdownGraphicsSystemImpl(); }
void DrawStart(void) { DrawStartImpl(); }
void DrawEnd(void) { DrawEndImpl(); }
void GraphicsWindowResizeEvent(int width, int height) { GraphicsWindowResizeEventImpl(width, height); }
void GraphicsSetLogicalWorldSize(int width, int height) { GraphicsSetLogicalWorldSizeImpl(width, height); }
int GraphicsGetTargetRefreshRate(void) { return GraphicsGetTargetRefreshRateImpl(); }
