#include <Supergoon/Graphics/graphics.h>

extern void InitializeGraphicsSystemImpl(void);
extern void ShutdownGraphicsSystemImpl(void);
extern void DrawEndImpl(void);
extern void DrawStartImpl(void);
extern void GraphicsWindowResizeEventImpl(int width, int height);
extern void GraphicsSetLogicalWorldSizeImpl(int width, int height);
extern int GraphicsGetTargetRefreshRateImpl(void);
extern void DrawRectImpl(RectangleF* rect, Color* color, int filled, int cameraOffset);
extern void GraphicsUpdateFBOColorImpl(Color* color);
extern Color GraphicsGetFBOColorImpl(void);
extern void* GraphicsGetContextPtrImpl(void);

void DrawRect(RectangleF* rect, Color* color, int filled, int cameraOffset) { DrawRectImpl(rect, color, filled, cameraOffset); }
void InitializeGraphicsSystem(void) { InitializeGraphicsSystemImpl(); }
void ShutdownGraphicsSystem(void) { ShutdownGraphicsSystemImpl(); }
void DrawStart(void) { DrawStartImpl(); }
void DrawEnd(void) { DrawEndImpl(); }
void GraphicsWindowResizeEvent(int width, int height) { GraphicsWindowResizeEventImpl(width, height); }
void GraphicsSetLogicalWorldSize(int width, int height) { GraphicsSetLogicalWorldSizeImpl(width, height); }
int GraphicsGetTargetRefreshRate(void) { return GraphicsGetTargetRefreshRateImpl(); }
void GraphicsUpdateFBOColor(Color* color) { GraphicsUpdateFBOColorImpl(color); }
Color GraphicsGetFBOColor(void) { return GraphicsGetFBOColorImpl(); }
void* GraphicsGetContextPtr(void) { return GraphicsGetContextPtrImpl(); }
