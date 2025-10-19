#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TUIDrawCallback)(void* parent, void* child);
void* PanelCreate(int width, int height, int x, int y);
void* PanelGetCursesWindow(void* panelPtr);
void PanelUpdateSize(void* panelPtr, int w, int h);
void PanelUpdateLocation(void* panelPtr, int w, int h);
void PanelAddBorder(void* panelPtr, int shouldBorder);
void PanelAddName(void* panelPtr, const char* name);
void PanelDraw(void* panelPtr);
void PanelAddChild(void* parentPanelPtr, void* childPtr, TUIDrawCallback func);
void PanelDestroy(void* panelPtr);
void PanelSetFocus(void* panelPtr, int shouldFocus);

#ifdef __cplusplus
}
#endif
