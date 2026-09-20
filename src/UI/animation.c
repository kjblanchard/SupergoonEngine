#include <Supergoon/Animation/animator.h>
#include <Supergoon/UI/animation.h>
#include <Supergoon/sprite.h>
#include <string.h>

static void UIAnimatorDraw(UIObject* o) {
	UIAnimatorData* data = (UIAnimatorData*)o->TypeData;
	if (!data) {
		return;
	}
	data->Sprite->ParentX = o->Rect.x + o->ParentOffset.X;
	data->Sprite->ParentY = o->Rect.y + o->ParentOffset.Y;
	SpriteDrawManual(data->Sprite, &o->Rect, &(Color){255, 255, 255, 255}, false);
}

static void UIAnimatorDestroy(UIObject* o) {
	UIAnimatorData* data = (UIAnimatorData*)o->TypeData;
	if (!data) {
		return;
	}
	DestroyAnimator(data->Animator);
	free(o->TypeData);
}

static UIType uiAnimatorType = {"Animator", NULL, UIAnimatorDraw, UIAnimatorDestroy};

UIObject* UICreateAnimator(UIAnimatorArgs* args) {
	UIAnimatorData* data = malloc(sizeof(UIAnimatorData));
	args->Object->Type = &uiAnimatorType;
	args->Object->TypeData = data;
	data->Sprite = args->Sprite;
	data->Animator = args->Animator;
	return args->Object;
	return NULL;
}
