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
// extraFlags are ImGuiWindowFlags OR'd onto the shared window flags.
void DebugWindowAddWindowFuncWithFlags(const char* windowName, DebugWindowDrawFunc drawFunc, int extraFlags);
#ifdef __cplusplus
}
#endif
