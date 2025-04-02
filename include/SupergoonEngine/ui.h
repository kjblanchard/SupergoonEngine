#pragma once
#include <Supergoon/UI/ui.h>
#ifdef __cplusplus
extern "C" {
#endif
void InitializeUISystem(void);
void UpdateUISystem(void);
void DrawUISystem(void);
void ShutdownUISystem(void);
#ifdef __cplusplus
}
#endif
