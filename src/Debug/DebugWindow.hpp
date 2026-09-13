#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef void (*DebugWindowDrawFunc)(void);
void DebugSystemInitialize();
void DebugSystemHandleEvent(void* e);
void DebugSystemDraw();
void DebugSystemRender();
void DebugWindowAddTabFuncToMainDebugWindow(DebugWindowDrawFunc drawFunc);
void DebugWindowAddWindowFunc(const char* windowName, DebugWindowDrawFunc drawFunc);
#ifdef __cplusplus
}
#endif
