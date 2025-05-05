#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <Supergoon/events.h>

void InitializeImGui(void);
// Returns true if we should send this to the game, false if we should continue
bool HandleImGuiEvent(Event* event);
void StartImGuiFrame(void);
void DrawImGui(void);
void EndImGuiFrame(void);
void ShutdownImGui(void);
#ifdef __cplusplus
}
#endif
