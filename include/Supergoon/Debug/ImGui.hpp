#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef union SDL_Event Event;

void InitializeImGui(void);
// Returns true if we should send this to the game, false if we should continue
bool HandleImGuiEvent(Event* event);
void StartImGuiFrame(void);
void DrawImGui(void);
void EndImGuiFrame(void);
#ifdef __cplusplus
}
#endif
