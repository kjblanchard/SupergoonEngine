#pragma once
#include <Supergoon/UI/object.h>
#ifdef __cplusplus
extern "C" {
#endif

struct Sprite;
struct Animator;

typedef struct UIAnimatorData {
	struct Sprite* Sprite;
	struct Animator* Animator;
} UIAnimatorData;

typedef struct UIAnimatorArgs {
	UIObject* Object;
	struct Sprite* Sprite;
	struct Animator* Animator;
} UIAnimatorArgs;

//Help me
UIObject* UICreateAnimator(UIAnimatorArgs* args);

#ifdef __cplusplus
}
#endif
