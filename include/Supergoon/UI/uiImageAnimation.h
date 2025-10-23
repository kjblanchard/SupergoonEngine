
#pragma once
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/UI/uiobject.h>
#ifdef __cplusplus
extern "C" {
#endif
struct Sprite;
typedef struct UIImageAnimationData {
	struct Sprite* SpritePtr;
	unsigned int AnimatorHandle;

} UIImageAnimationData;

void DrawUIImageAnimation(UIObject* uiobject);
void DestroyUIImageAnimation(UIObject* uiobject);
#ifdef __cplusplus
}
#endif
