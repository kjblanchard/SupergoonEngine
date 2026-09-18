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
#pragma once
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
	int CurrentAnimNum;
	int NextAnimNum[MAX_NUM_ANIM_QUEUE];
	int NextAnimLoops[MAX_NUM_ANIM_QUEUE];
	float CurrentFrameTime;
	bool Reverse;
	int Loops;
	AnimationData* Data;
	Sprite* Sprite;
	float AnimationSpeed;
} Animator;

Animator* CreateAnimator(const char* filename, AnimationData* data);
void PlayAnimation(Animator* a, const char* anim, int loops);
void AddAnimationToAnimatorQueue(Animator* a, const char* animName, int loops);
void DestroyAnimator(Animator* a);
void UpdateAnimatorSystem(void);
void ShutdownAnimationSystem(void);

#ifdef __cplusplus
}
#endif
