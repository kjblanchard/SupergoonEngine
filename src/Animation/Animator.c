#include <Supergoon/Animation/animator.h>
#include <Supergoon/lua.h>
#include <Supergoon/state.h>
#include <SupergoonEngine/Animation/animator.h>
#include <SupergoonEngine/tools.h>
#include <assert.h>

AnimationDataArray _animationData;
AnimatorArray _animators;

static void cleanUsedAnimationData(AnimationData* data) {
	for (size_t i = 0; i < data->frameCount; i++) {
		SDL_free(data->frames[i].filename);
		data->frames[i].filename = NULL;
	}
	SDL_free(data->frames);
	data->frames = NULL;
	for (size_t i = 0; i < data->meta.frameTagCount; i++) {
		SDL_free(data->meta.frameTags[i].name);
		data->meta.frameTags[i].name = NULL;
	}
	SDL_free(data->meta.frameTags);
	data->meta.frameTags = NULL;
	SDL_free(data->meta.image);
	data->meta.image = NULL;
	data->meta.frameTagCount = 0;
}

// Finds free animation data and increases ref count
static AnimationData* findFreeAnimationData(void) {
	for (size_t i = 0; i < _animationData.Size; i++) {
		if (_animationData.AnimationArray[i].RefCount == 0) {
			// create new animation data
			cleanUsedAnimationData(&_animationData.AnimationArray[i].Data);
			++_animationData.AnimationArray[i].RefCount;
			return &_animationData.AnimationArray[i].Data;
		}
	}
	RESIZE_ARRAY(_animationData.AnimationArray, _animationData.Count, _animationData.Size, AnimationDataRef);
	AnimationDataRef* ref = &_animationData.AnimationArray[_animationData.Count];
	memset(ref, 0, sizeof(AnimationDataRef));
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
				if (_animationData.AnimationArray[i].RefCount == 0) {
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
	for (size_t i = 0; i < _animationData.Count; i++) {
		AnimationDataRef* currentPtr = &_animationData.AnimationArray[i];
		if (currentPtr->RefCount == 0) {
			continue;
		}
		if (strcmpWithSuffix(name, currentPtr->Data.meta.image, ".bmp")) {
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

		} else {
			animationData->meta.frameTags[i].direction = AnimationDataDirectionsDefault;
		}
		LuaPopStack(_luaState, 1);
	}
	LuaPopStack(_luaState, 1);
}

static AnimatorHandle getFreeAnimator(void) {
	for (size_t i = 0; i < _animators.Size; i++) {
		if (!_animators.Animators[i].Data) {
			memset(&_animators.Animators[i], 0, sizeof(Animator));
			return i;
		}
	}
	RESIZE_ARRAY(_animators.Animators, _animators.Count, _animators.Size, Animator);
	memset(&_animators.Animators[_animators.Count], 0, sizeof(Animator));
	return _animators.Count++;
}

AnimatorHandle CreateAnimator(const char* filename) {
	AnimatorHandle handle = getFreeAnimator();
	assert(handle < _animators.Size && "Size not correct");
	Animator* anim = &_animators.Animators[handle];
	anim->Name = strdup(filename);
	asprintf(&anim->Filename, "assets/aseprite/%s.lua", filename);
	anim->Data = findAnimationData(filename);
	if (!anim->Data) {
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

void PlayAnimation(AnimatorHandle animator, const char* anim) {
	Animator* animatorPtr = &_animators.Animators[animator];
	if (!animatorPtr || !animatorPtr->Data) {
		sgLogWarn("Could not play animation, bad animator");
	}
	for (size_t i = 0; i < animatorPtr->Data->meta.frameTagCount; i++) {
		if (strcmp(animatorPtr->Data->meta.frameTags[i].name, anim) == 0) {
			animatorPtr->CurrentAnimNum = i;
			animatorPtr->CurrentFrame = animatorPtr->Data->meta.frameTags[i].from;
			animatorPtr->CurrentFrameTime = 0;
			// TODO the loops are always -1?  not implemented
			animatorPtr->Loops = -1;
			// TODO speed is always 1.0 right now, not implemented
			animatorPtr->AnimationSpeed = 1.0f;
			updateAnimatorRect(animatorPtr);
		}
	}
}

void DestroyAnimator(AnimatorHandle animator) {
	if (animator >= _animators.Count) {
		sgLogWarn("Could not destroy, count not right");
		return;
	}
	Animator* anim = &_animators.Animators[animator];
	assert(anim && "No anim");
	SDL_free(anim->Name);
	SDL_free(anim->Filename);
	DestroySprite(anim->Sprite);
	removeAnimationDataRef(anim->Data);
	anim->Data = NULL;
}

void updateAnimator(Animator* animator) {
	if (animator->Loops == 0 || animator->AnimationSpeed == 0.0f) {
		return;
	}
	// bool justFinished = false;
	animator->CurrentFrameTime += DeltaTimeMilliseconds * animator->AnimationSpeed;
	Frame* frameData = &animator->Data->frames[animator->CurrentFrame];
	FrameTag* animData = &animator->Data->meta.frameTags[animator->CurrentAnimNum];
	// bool progressed = false;
	//   // use a while loop incase the deltat time is long
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
					// justFinished = true;
					animator->Reverse = false;
				}
			} else {
				animator->NextFrame = animator->CurrentFrame + 1;
				if (animator->NextFrame > animData->to) {
					if (animator->Loops != -1) {
						--animator->Loops;
					}
					animator->NextFrame = animData->to - 1;
					// justFinished = true;
					animator->Reverse = true;
				}
			}
		} else {
			// handle other directions
			//     } else {
			//       if (animData.direction == "forward") {
			//         // If the next frame is longer than the end, loop to the beginning, else increment it
			//         _nextFrame = (_frame + 1 > animData.to) ? animData.from : _frame + 1;
			//         // If this would cause us to loop to the beginning, we should set just finished and decrement repeats
			//         if (_nextFrame == animData.from) {
			//           _repeats = _repeats == -1 ? -1 : _repeats - 1;
			//           justFinished = true;
			//         }
			//       } else if (animData.direction == "reverse") {
			//         _nextFrame = (_frame - 1 < animData.from) ? animData.to : _frame - 1;
			//         if (_nextFrame == animData.to) {
			//           _repeats = _repeats == -1 ? -1 : _repeats - 1;
			//           justFinished = true;
			//         }
			//       }
			//     }
		}
		//     // We should only update the rect, if we have any repeats left.
		//     if (_repeats == 0) {
		if (animator->Loops == 0) {
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
	if (speed != 0) {
		sgLogInfo("Setting animator animation speed to %f", speed);
	}
	_animators.Animators[animator].AnimationSpeed = speed;
}
