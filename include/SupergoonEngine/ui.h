#pragma once
#include <Supergoon/UI/ui.h>
#ifdef __cplusplus
extern "C" {
#endif
void InitializeUISystem(void);
void UpdateUISystem(void);
void DrawUISystem(void);
void ShutdownUISystem(void);
extern unsigned int _nextObjectId;
extern UIObject *_rootUIObject;
#ifdef __cplusplus
}
#endif
