#include <Supergoon/Animation/animator.h>
#include <Supergoon/state.h>
#include <assert.h>
#include <sgtools/log.h>
#include <sgtools/tools.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_NEXT_ANIM -1

typedef struct AnimatorArray {
	Animator** Animators;
	size_t Count;
	size_t Size;
} AnimatorArray;

AnimatorArray animators;

Animator* CreateAnimator(const char* filename, AnimationData* data) {
	Animator* a = calloc(1, sizeof(*a));
	a->Name = strdup(filename);
	asprintf(&a->Filename, "assets/aseprite/%s.json", filename);
	a->Data = data;
	ArrayResizeIfNeeded((void**)&animators.Animators, animators.Count + 1, &animators.Size, sizeof(Animator*));
	animators.Animators[animators.Count++] = a;
	return a;
}

static void updateAnimatorRect(Animator* a) {
	if (!a || !a->Sprite) {
		sgLogWarn("bad animator and or sprite for update");
	}
	a->Sprite->TextureSourceRect.x = (float)a->Data->frames[a->CurrentFrame].frame.x;
	a->Sprite->TextureSourceRect.y = (float)a->Data->frames[a->CurrentFrame].frame.y;
	a->Sprite->TextureSourceRect.h = (float)a->Data->frames[a->CurrentFrame].frame.h;
	a->Sprite->TextureSourceRect.w = (float)a->Data->frames[a->CurrentFrame].frame.w;
}

static void playAnimation(Animator* a, int animNum, int loops) {
	a->CurrentAnimNum = animNum;
	a->CurrentFrame = a->Data->meta.frameTags[animNum].from;
	a->CurrentFrameTime = 0;
	a->Loops = loops;
	updateAnimatorRect(a);
}

static int findAnimationNumberByName(Animator* a, const char* animName) {
	if (!a || !a->Data || !animName) {
		sgLogWarn("invalid anim");
		return NO_NEXT_ANIM;
	}
	for (int i = 0; i < a->Data->meta.frameTagCount; i++) {
		if (strcmp(a->Data->meta.frameTags[i].name, animName) == 0) {
			return i;
		}
	}
	sgLogWarn("Could not find animation with name %s", animName);
	return NO_NEXT_ANIM;
}

void PlayAnimation(Animator* a, const char* animName, int loops) {
	if (!a || !a->Data) {
		sgLogWarn("Could not play animation, bad animator");
	}
	int animToPlay = findAnimationNumberByName(a, animName);
	if (animToPlay == NO_NEXT_ANIM) {
		return;
	}
	playAnimation(a, animToPlay, loops);
}

void DestroyAnimator(Animator* a) {
	assert(a && "No anim");
	free(a->Name);
	free(a->Filename);
	SpriteDestroy(a->Sprite);
	for (size_t i = 0; i < animators.Count; ++i) {
		if (a == animators.Animators[i]) {
			ArrayRemoveFromUnsorted((void*)animators.Animators, &animators.Count, sizeof(Animator*), i);
			break;
		}
	}
	free(a);
}

void updateAnimator(Animator* animator) {
	if (!animator || animator->Loops == 0 || animator->AnimationSpeed == 0.0f) {
		return;
	}
	animator->CurrentFrameTime += DeltaTimeMilliseconds * animator->AnimationSpeed;
	Frame* frameData = &animator->Data->frames[animator->CurrentFrame];
	FrameTag* animData = &animator->Data->meta.frameTags[animator->CurrentAnimNum];
	while (animator->CurrentFrameTime >= (float)frameData->duration) {
		animator->CurrentFrameTime -= (float)frameData->duration;
		if (animData->direction == AnimationDataDirectionsPingPong) {
			if (animator->Reverse) {
				animator->NextFrame = animator->CurrentFrame - 1;
				if (animator->NextFrame < animData->from) {
					if (animator->Loops != -1) {
						--animator->Loops;
					}
					animator->NextFrame = animData->from + 1;
					animator->Reverse = false;
				}
			} else {
				animator->NextFrame = animator->CurrentFrame + 1;
				if (animator->NextFrame > animData->to) {
					if (animator->Loops != -1) {
						--animator->Loops;
					}
					animator->NextFrame = animData->to - 1;
					animator->Reverse = true;
				}
			}
		} else if (animData->direction == AnimationDataDirectionsForward) {
			animator->NextFrame = animator->CurrentFrame + 1;
			if (animator->NextFrame > animData->to) {
				if (animator->Loops != -1) {
					--animator->Loops;
				}
				animator->NextFrame = animData->from;
			}
		} else {
			sgLogWarn("Animator trying to handle a direction not implemented for %s", animData->name);
		}
		if (animator->Loops == 0) {
			if (animator->NextAnimNum[0] != NO_NEXT_ANIM) {
				playAnimation(animator, animator->NextAnimNum[0], animator->NextAnimLoops[0]);
				for (size_t i = 0; i < MAX_NUM_ANIM_QUEUE - 1; i++) {
					animator->NextAnimNum[i] = animator->NextAnimNum[i + 1];
					animator->NextAnimLoops[i] = animator->NextAnimLoops[i + 1];
					if (animator->NextAnimNum[i + 1] == NO_NEXT_ANIM) {
						break;
					}
				}
			}
			return;
		}
		animator->CurrentFrame = animator->NextFrame;
		frameData = &animator->Data->frames[animator->CurrentFrame];
		updateAnimatorRect(animator);
	}
}

void UpdateAnimatorSystem(void) {
	for (size_t i = 0; i < animators.Count; i++) {
		if (animators.Animators[i]->Data)
			updateAnimator(animators.Animators[i]);
	}
}

void AddAnimationToAnimatorQueue(Animator* a, const char* animName, int loops) {
	if (!a) {
		return;
	}
	int animNum = findAnimationNumberByName(a, animName);
	if (animNum == NO_NEXT_ANIM) {
		return;
	}
	for (size_t i = 0; i < MAX_NUM_ANIM_QUEUE; i++) {
		if (a->NextAnimNum[i] != NO_NEXT_ANIM) {
			continue;
		}
		sgLogDebug("Adding in anim %s to queue at pos %d", animName, i);
		a->NextAnimNum[i] = animNum;
		a->NextAnimLoops[i] = loops;
		return;
	}
	sgLogWarn("Could not add animation %s to animator queue of %s, because it is full!", animName, a->Name);
}

void ShutdownAnimationSystem(void) {
}
