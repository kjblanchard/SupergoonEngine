#include <Supergoon/Animation/animator.h>
#include <Supergoon/log.h>
#include <Supergoon/state.h>
#include <Supergoon/tools.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_NEXT_ANIM -1
static size_t _firstAnimatorHole = NO_HOLE;

typedef struct AnimatorArray {
	Animator** Animators;
	size_t Count;
	size_t Size;
} AnimatorArray;

AnimatorArray _animators;

// Used when reusing a old animator to reset anim queue
static void setNewAnim(Animator* anim) {
	CLEAR_STRUCT(anim);
	for (size_t j = 0; j < MAX_NUM_ANIM_QUEUE; j++) {
		anim->NextAnimNum[j] = NO_NEXT_ANIM;
		anim->NextAnimLoops[j] = NO_NEXT_ANIM;
	}
}

static Animator* getFreeAnimator(void) {
	if (_firstAnimatorHole == NO_HOLE) {
		RESIZE_ARRAY_PTR_ALLOC(_animators.Animators, _animators.Count, _animators.Size, Animator);
		return _animators.Animators[_animators.Count++];
	}
	Animator* returnSprite = _animators.Animators[_firstAnimatorHole];
	setNewAnim(returnSprite);
	size_t nextHole = NO_HOLE;
	for (size_t i = _firstAnimatorHole + 1; i < _animators.Count; i++) {
		if (_animators.Animators[i]->IsDestroyed) {
			nextHole = i;
			break;
		}
	}
	_firstAnimatorHole = nextHole;
	return returnSprite;
}

Animator* CreateAnimator(const char* filename, AnimationData* data) {
	Animator* anim = getFreeAnimator();
	assert(anim && "Could not get free anim");
	anim->Name = strdup(filename);
	asprintf(&anim->Filename, "assets/aseprite/%s.json", filename);
	anim->Data = data;
	return anim;
}

static void updateAnimatorRect(Animator* animator) {
	if (!animator || animator->Sprite) {
		sgLogWarn("bad animator and or sprite for assert");
	}
	animator->Sprite->TextureSourceRect.x = animator->Data->frames[animator->CurrentFrame].frame.x;
	animator->Sprite->TextureSourceRect.y = animator->Data->frames[animator->CurrentFrame].frame.y;
	animator->Sprite->TextureSourceRect.h = animator->Data->frames[animator->CurrentFrame].frame.h;
	animator->Sprite->TextureSourceRect.w = animator->Data->frames[animator->CurrentFrame].frame.w;
}

static void playAnimation(Animator* anim, int animNum, int loops) {
	anim->CurrentAnimNum = animNum;
	anim->CurrentFrame = anim->Data->meta.frameTags[animNum].from;
	anim->CurrentFrameTime = 0;
	anim->Loops = loops;
	// TODO speed is always 1.0 right now, not implemented
	anim->AnimationSpeed = 1.0f;
	updateAnimatorRect(anim);
}

static int findAnimationNumberByName(Animator* anim, const char* animName) {
	if (!anim || !anim->Data) {
		sgLogWarn("Trying to find anim to play on bad anim");
		return -1;
	}
	for (size_t i = 0; i < anim->Data->meta.frameTagCount; i++) {
		if (strcmp(anim->Data->meta.frameTags[i].name, animName) == 0) {
			return i;
		}
	}
	sgLogWarn("Could not find animation with name %s", animName);
	return -1;
}

void PlayAnimation(Animator* animator, const char* anim, int loops) {
	if (!animator || !animator->Data) {
		sgLogWarn("Could not play animation, bad animator");
	}
	int animToPlay = findAnimationNumberByName(animator, anim);
	if (animToPlay == -1) {
		return;
	}
	playAnimation(animator, animToPlay, loops);
}

void DestroyAnimator(Animator* animator) {
	assert(animator && "No anim");
	free(animator->Name);
	animator->Name = NULL;
	free(animator->Filename);
	animator->Filename = NULL;
	DestroySprite(animator->Sprite);
	animator->Data = NULL;
	for (size_t i = 0; i < MAX_NUM_ANIM_QUEUE; i++) {
		animator->NextAnimNum[i] = NO_NEXT_ANIM;
		animator->NextAnimLoops[i] = -1;
	}
}

void updateAnimator(Animator* animator) {
	if (animator->Loops == 0 || animator->AnimationSpeed == 0.0f) {
		return;
	}
	animator->CurrentFrameTime += DeltaTimeMilliseconds * animator->AnimationSpeed;
	Frame* frameData = &animator->Data->frames[animator->CurrentFrame];
	FrameTag* animData = &animator->Data->meta.frameTags[animator->CurrentAnimNum];
	// bool progressed = false;
	//   // use a while loop incase the delta time is long
	while (animator->CurrentFrameTime >= frameData->duration) {
		// progressed = true;
		animator->CurrentFrameTime -= frameData->duration;
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

void UpdateAnimators(void) {
	for (size_t i = 0; i < _animators.Count; i++) {
		if (_animators.Animators[i]->Data)
			updateAnimator(_animators.Animators[i]);
	}
}

void AddAnimationToAnimatorQueue(Animator* animator, const char* animName, int loops) {
	if (!animator) {
		return;
	}
	int animNum = findAnimationNumberByName(animator, animName);
	if (animNum == -1) {
		return;
	}
	for (size_t i = 0; i < MAX_NUM_ANIM_QUEUE; i++) {
		if (animator->NextAnimNum[i] != -1) {
			continue;
		}
		animator->NextAnimNum[i] = animNum;
		animator->NextAnimLoops[i] = loops;
		return;
	}
	sgLogWarn("Could not add animation %s to animator queue of %s, because it is full!", animName, animator->Name);
}

void ShutdownAnimationSystem(void) {
}
