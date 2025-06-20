#pragma once
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/sprite.h>
#include <SupergoonEngine/Animation/animationdata.h>
#include <stdbool.h>
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct Animator {
	char* Name;
	char* Filename;
	int CurrentFrame;
	int NextFrame;
	unsigned int CurrentAnimNum;
	float CurrentFrameTime;
	bool Reverse;
	int Loops;
	AnimationData* Data;
	Sprite* Sprite;
	float AnimationSpeed;

} Animator;

typedef struct AnimatorArray {
	Animator* Animators;
	size_t Count;
	size_t Size;

} AnimatorArray;

typedef struct AnimationDataRef {
	AnimationData Data;
	size_t RefCount;
} AnimationDataRef;

// If refcount is 0, it can be reused.
typedef struct AnimationDataArray {
	AnimationDataRef* AnimationArray;
	size_t Count;
	size_t Size;
} AnimationDataArray;

extern AnimationDataArray _animationData;
extern AnimatorArray _animators;
void UpdateAnimators(void);
#ifdef __cplusplus
}
#endif
