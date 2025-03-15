#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef union SDL_Event Event;

void InitializeImGui(void);
void HandleImGuiEvent(Event* event);
void StartImGuiFrame(void);
void DrawImGui(void);
void EndImGuiFrame(void);
#ifdef __cplusplus
}
#endif
