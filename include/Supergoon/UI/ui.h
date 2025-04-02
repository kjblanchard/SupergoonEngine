#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UIObject UIObject;

void AddUIObject(UIObject* child, UIObject* parent);
#ifdef __cplusplus
}
#endif
