#include <Supergoon/Animation/animator.h>
#include <Supergoon/UI/uiImageAnimation.h>
#include <Supergoon/sprite.h>

void DrawUIImageAnimation(UIObject* uiobject) {
	UIImageAnimationData* data = (UIImageAnimationData*)uiobject->Data;
	if (!data || !data->SpritePtr) {
		return;
	}
	RectangleF* src = (data->SpritePtr->TextureSourceRect.x == 0 && data->SpritePtr->TextureSourceRect.y == 0 && data->SpritePtr->TextureSourceRect.w == 0 && data->SpritePtr->TextureSourceRect.h == 0) ? NULL : &data->SpritePtr->TextureSourceRect;
	DrawTexture(data->SpritePtr->Texture, &uiobject->Location, src);
}

void DestroyUIImageAnimation(UIObject* uiobject) {
	UIImageAnimationData* data = (UIImageAnimationData*)uiobject->Data;
	if (!data) {
		return;
	}
	DestroySprite(data->SpritePtr);
	DestroyAnimator(data->AnimatorHandle);
}
