#include <Supergoon/Animation/animator.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/state.h>
#include <Supergoon/tools.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_NEXT_ANIM -1
AnimationDataArray _animationData;
AnimatorArray _animators;

static void cleanUsedAnimationData(AnimationData* data) {
	if (data->frames) {
		for (size_t i = 0; i < data->frameCount; i++) {
			free(data->frames[i].filename);
			data->frames[i].filename = NULL;
		}
		free(data->frames);
		data->frames = NULL;
	}
	if (data->meta.frameTags) {
		for (size_t i = 0; i < data->meta.frameTagCount; i++) {
			free(data->meta.frameTags[i].name);
			data->meta.frameTags[i].name = NULL;
		}
		free(data->meta.frameTags);
		data->meta.frameTags = NULL;
	}
	if (data->meta.image) {
		free(data->meta.image);
		data->meta.image = NULL;
	}
	data->meta.frameTagCount = 0;
	data->frameCount = 0;
}

// Finds free animation data and increases ref count
static AnimationData* findFreeAnimationData(void) {
	for (size_t i = 0; i < _animationData.Size; i++) {
		if (_animationData.AnimationArray[i].RefCount == 0) {
			sgLogWarn("Found one to use!");
			// create new animation data
			cleanUsedAnimationData(&_animationData.AnimationArray[i].Data);
			++_animationData.AnimationArray[i].RefCount;
			return &_animationData.AnimationArray[i].Data;
		}
	}
	sgLogWarn("Resizing array");
	RESIZE_ARRAY(_animationData.AnimationArray, _animationData.Count, _animationData.Size, AnimationDataRef);
	AnimationDataRef* ref = &_animationData.AnimationArray[_animationData.Count];
	ref->RefCount = 1;
	++_animationData.Count;
	return &ref->Data;
}

// Removes ref count of a ref data ptr
static void removeAnimationDataRef(AnimationData* data) {
	for (size_t i = 0; i < _animationData.Count; i++) {
		AnimationDataRef* thing = &_animationData.AnimationArray[i];
		if (&thing->Data == data) {
			if (_animationData.AnimationArray[i].RefCount > 0) {
				--_animationData.AnimationArray[i].RefCount;
				if (_animationData.AnimationArray[i].RefCount < 1) {
					cleanUsedAnimationData(data);
				}
			} else {
				sgLogWarn("RefCount underflow attempted on animation data");
			}
			return;
		}
	}
	sgLogWarn("Trying to remove ref to something not found");
}
// Finds a animation data based on name
static AnimationData* findAnimationData(const char* name) {
	for (size_t i = 0; i < _animationData.Size; i++) {
		AnimationDataRef* currentPtr = &_animationData.AnimationArray[i];
		if (currentPtr->RefCount == 0) {
			continue;
		}
		if (strcmpWithSuffix(name, currentPtr->Data.meta.image, ".bmp")) {
			sgLogWarn("Found one with strcmp");
			++currentPtr->RefCount;
			return &_animationData.AnimationArray[i].Data;
		}
	}
	return NULL;
}

// Aseprite table must be on stack prior to calling this, then it
static void loadAsepriteData(AnimationData* animationData) {
	LuaGetTable(_luaState, "frames");
	animationData->frameCount = LuaGetTableLength(_luaState);
	animationData->frames = calloc(animationData->frameCount, sizeof(Frame));
	for (size_t i = 0; i < animationData->frameCount; i++) {
		Frame* frame = &animationData->frames[i];
		LuaPushTableObjectToStacki(_luaState, i);
		frame->duration = LuaGetInt(_luaState, "duration");
		LuaGetTable(_luaState, "frame");
		frame->frame.x = LuaGetInt(_luaState, "x");
		frame->frame.y = LuaGetInt(_luaState, "y");
		frame->frame.w = LuaGetInt(_luaState, "w");
		frame->frame.h = LuaGetInt(_luaState, "h");
		LuaPopStack(_luaState, 1);
		frame->rotated = LuaGetBool(_luaState, "rotated");
		LuaGetTable(_luaState, "sourceSize");
		frame->sourceSize.h = LuaGetInt(_luaState, "h");
		frame->sourceSize.w = LuaGetInt(_luaState, "w");
		LuaPopStack(_luaState, 1);
		LuaPopStack(_luaState, 1);
	}
	LuaPopStack(_luaState, 1);
	LuaGetTable(_luaState, "meta");
	animationData->meta.image = LuaAllocateString(_luaState, "image");
	LuaGetTable(_luaState, "frameTags");
	animationData->meta.frameTagCount = LuaGetTableLength(_luaState);
	animationData->meta.frameTags = calloc(animationData->meta.frameTagCount, sizeof(FrameTag));
	for (size_t i = 0; i < animationData->meta.frameTagCount; i++) {
		LuaPushTableObjectToStacki(_luaState, i);
		animationData->meta.frameTags[i].name = LuaAllocateString(_luaState, "name");
		animationData->meta.frameTags[i].from = LuaGetInt(_luaState, "from");
		animationData->meta.frameTags[i].to = LuaGetInt(_luaState, "to");
		const char* direction = LuaGetString(_luaState, "direction");
		if (strcmp(direction, "pingpong") == 0) {
			animationData->meta.frameTags[i].direction = AnimationDataDirectionsPingPong;

		} else if (strcmp(direction, "forward") == 0) {
			animationData->meta.frameTags[i].direction = AnimationDataDirectionsForward;
		} else {
			animationData->meta.frameTags[i].direction = AnimationDataDirectionsDefault;
		}
		LuaPopStack(_luaState, 1);
	}
	LuaPopStack(_luaState, 1);
}

static void setNewAnim(Animator* anim) {
	CLEAR_STRUCT(anim);
	for (size_t j = 0; j < MAX_NUM_ANIM_QUEUE; j++) {
		anim->NextAnimNum[j] = NO_NEXT_ANIM;
		anim->NextAnimLoops[j] = NO_NEXT_ANIM;
	}
}

static AnimatorHandle getFreeAnimator(void) {
	for (size_t i = 0; i < _animators.Size; i++) {
		if (!_animators.Animators[i].Data) {
			setNewAnim(&_animators.Animators[i]);
			if (i >= _animators.Count) {
				++_animators.Count;
			}
			return i;
		}
	}
	int currentSize = _animators.Size;
	RESIZE_ARRAY(_animators.Animators, _animators.Count, _animators.Size, Animator);
	// Set new items to have no anim for the next anim.
	if (currentSize < _animators.Size) {
		for (size_t i = currentSize; i < _animators.Size; i++) {
			Animator* anim = &_animators.Animators[i];
			setNewAnim(anim);
		}
	}
	return _animators.Count++;
}

AnimatorHandle CreateAnimator(const char* filename) {
	AnimatorHandle handle = getFreeAnimator();
	assert(handle < _animators.Size && "Size not correct");
	Animator* anim = &_animators.Animators[handle];
	anim->Name = strdup(filename);
	asprintf(&anim->Filename, "assets/aseprite/%s.lua", filename);
	sgLogWarn("Searching for anim data %s", anim->Filename);
	anim->Data = findAnimationData(filename);
	if (!anim->Data) {
		sgLogWarn("None found, going to create new");
		anim->Data = findFreeAnimationData();
		LuaPushTableFromFile(_luaState, anim->Filename);
		loadAsepriteData(anim->Data);
		LuaPopStack(_luaState, 1);
	}
	return handle;
}

static void updateAnimatorRect(Animator* animator) {
	assert(animator && animator->Sprite && "bad animator and or sprite for assert");
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
	for (size_t i = 0; i < anim->Data->meta.frameTagCount; i++) {
		if (strcmp(anim->Data->meta.frameTags[i].name, animName) == 0) {
			return i;
		}
	}
	sgLogWarn("Could not find animation with name %s", animName);
	return -1;
}

void PlayAnimation(AnimatorHandle animator, const char* anim, int loops) {
	if (animator > _animators.Count) {
		sgLogWarn("Passed animator handle greater than count, returning!");
		return;
	}
	Animator* animatorPtr = &_animators.Animators[animator];
	if (!animatorPtr || !animatorPtr->Data) {
		sgLogWarn("Could not play animation, bad animator");
	}
	int animToPlay = findAnimationNumberByName(animatorPtr, anim);
	if (animToPlay == -1) {
		return;
	}
	playAnimation(animatorPtr, animToPlay, loops);
}

void DestroyAnimator(AnimatorHandle animator) {
	if (animator >= _animators.Count) {
		sgLogWarn("Could not destroy, count not right");
		return;
	}
	Animator* anim = &_animators.Animators[animator];
	assert(anim && "No anim");
	free(anim->Name);
	anim->Name = NULL;
	free(anim->Filename);
	anim->Filename = NULL;
	DestroySprite(anim->Sprite);
	removeAnimationDataRef(anim->Data);
	anim->Data = NULL;
	for (size_t i = 0; i < MAX_NUM_ANIM_QUEUE; i++) {
		anim->NextAnimNum[i] = NO_NEXT_ANIM;
		anim->NextAnimLoops[i] = -1;
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
		if (_animators.Animators[i].Data)
			updateAnimator(&_animators.Animators[i]);
	}
}

void SetAnimatorAnimationSpeed(AnimatorHandle animator, float speed) {
	_animators.Animators[animator].AnimationSpeed = speed;
}

void AddAnimationToAnimatorQueue(AnimatorHandle animator, const char* animName, int loops) {
	Animator* anim = &_animators.Animators[animator];
	if (!anim) {
		return;
	}
	int animNum = findAnimationNumberByName(anim, animName);
	if (animNum == -1) {
		return;
	}
	for (size_t i = 0; i < MAX_NUM_ANIM_QUEUE; i++) {
		if (anim->NextAnimNum[i] != -1) {
			continue;
		}
		anim->NextAnimNum[i] = animNum;
		anim->NextAnimLoops[i] = loops;
		return;
	}
	sgLogWarn("Could not add animation %s to animator queue of %s, because it is full!", animName, anim->Name);
}
