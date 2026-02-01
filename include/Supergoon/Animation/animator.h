/**
 * @file Animator.h
 * @author kevin blanchard (kevin@supergoon.com)
 * @brief Animators are used for heach aseprite file and handles gametime updates to all Instances of this, as it holds the animation data, and the list
 * @version 0.1
 * @date 2025-06-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifdef __cplusplus
extern "C" {
#endif
#include <Supergoon/Animation/animationdata.h>
#include <Supergoon/sprite.h>
#include <stdbool.h>
#include <stdlib.h>
#define MAX_NUM_ANIM_QUEUE 3
typedef struct Animator {
	char* Name;
	char* Filename;
	int CurrentFrame;
	int NextFrame;
	unsigned int CurrentAnimNum;
	// Allows for animations to play consecutively when they finish.
	int NextAnimNum[MAX_NUM_ANIM_QUEUE];
	int NextAnimLoops[MAX_NUM_ANIM_QUEUE];
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

typedef unsigned int AnimatorHandle;

AnimatorHandle CreateAnimator(const char* filename);
void PlayAnimation(AnimatorHandle animator, const char* anim, int loops);
// 1.0 is regular, 0.0 is none, etc
void SetAnimatorAnimationSpeed(AnimatorHandle animator, float speed);
void AddAnimationToAnimatorQueue(AnimatorHandle animator, const char* animName, int loops);
void DestroyAnimator(AnimatorHandle animator);
void ShutdownAnimationSystem(void);

#ifdef __cplusplus
}
#endif
