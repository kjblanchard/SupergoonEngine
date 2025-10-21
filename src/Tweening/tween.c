#include <SDL3/SDL_events.h>
#include <Supergoon/Tweening/easing.h>
#include <Supergoon/Tweening/tween.h>
#include <Supergoon/events.h>
#include <Supergoon/log.h>
#include <Supergoon/state.h>
#include <Supergoon/tween.h>
#include <stdbool.h>
#include <stdlib.h>

// #include <Supergoon/Tweening/easing.h>
// #include <Supergoon/Tweening/tween.h>
// #include <Supergoon/log.h>
// #include <stdbool.h>
// #include <stdlib.h>

static void updateTween(sgTween* tweenPtr, Tween id);
static void zeroTween(Tween tween);
static void updateTweenData(sgTween* tweenPtr);

// Tweens allocated by the engine
sgTween _tweens[MAX_TWEENS];
int _numberTweens = 0;

void initializeTweenEngine(void) {
	for (size_t i = 0; i < MAX_TWEENS; i++) {
		zeroTween(i);
	}
}

static bool isTweenAvailableForActions(Tween tween) {
	if (!_tweens[tween].Available) {
		return true;
	}
	sgLogInfo("Trying to perform actions on a tween that is marked as available (not in use)");
	return false;
}

void updateTweens(void) {
	for (size_t i = 0; i < (size_t)_numberTweens; i++) {
		if (_tweens[i].Available || !_tweens[i].Started) continue;
		updateTween(&_tweens[i], i);
	}
}

static Tween getFirstUnusedTween(void) {
	for (size_t i = 0; i < MAX_TWEENS; i++) {
		if (_tweens[i].Available) {
			if ((int)i >= _numberTweens) {
				++_numberTweens;
			}
			zeroTween(i);
			_tweens[i].Available = false;
			return i;
		}
	}
	sgLogWarn("Could not allocate tween, should you increase MAX_TWEENS or.. ? returning 0");
	return 0;
}

static void zeroTween(Tween tween) {
	sgTween* tweenPtr = &_tweens[tween];
	tweenPtr->Available = true;
	tweenPtr->CurrentTimeSeconds = 0;
	tweenPtr->EaseType = TweenEasingLinear;
	tweenPtr->EndFunc = NULL;
	tweenPtr->StartFunc = NULL;
	tweenPtr->UpdateFunc = NULL;
	tweenPtr->Flags = 0;
	tweenPtr->Loops = 0;
	tweenPtr->Started = false;
	tweenPtr->TotalTimeSeconds = 0;
	tweenPtr->Userdata = NULL;
}

static void updateTween(sgTween* tweenPtr, Tween id) {
	if (!isTweenAvailableForActions(id)) {
		return;
	}
	if (tweenPtr->CurrentTimeSeconds == 0 && tweenPtr->StartFunc) {
		tweenPtr->StartFunc(tweenPtr->Userdata);
	}
	tweenPtr->CurrentTimeSeconds += DeltaTimeSeconds;
	if (tweenPtr->UpdateFunc) {
		tweenPtr->UpdateFunc(tweenPtr->Userdata);
	}
	updateTweenData(tweenPtr);
	if (tweenPtr->CurrentTimeSeconds >= tweenPtr->TotalTimeSeconds) {
		if (tweenPtr->EndFunc) {
			tweenPtr->EndFunc(tweenPtr->Userdata);
		}
		tweenPtr->Started = false;
		tweenPtr->Available = true;
		tweenPtr->CurrentTimeSeconds = 0;
	}
}

static void updateTweenData(sgTween* tweenPtr) {
	float progress = 0;
	switch (tweenPtr->EaseType) {
		case TweenEasingLinear:
			progress = geLinearInterpolation(tweenPtr->CurrentTimeSeconds / tweenPtr->TotalTimeSeconds);
			break;
		default:
			sgLogError("Ease type not implemented, %d!", tweenPtr->EaseType);
			return;
	}
	if (tweenPtr->TweenType == TweenTypeFloat) {
		float progressVal = tweenPtr->Data.FloatData.Begin + ((tweenPtr->Data.FloatData.End - tweenPtr->Data.FloatData.Begin) * progress);
		if (tweenPtr->CurrentTimeSeconds > tweenPtr->TotalTimeSeconds) {
			progressVal = tweenPtr->Data.FloatData.End;
		}
		*tweenPtr->Data.FloatData.Value = progressVal;
	} else {
		float progressVal = tweenPtr->Data.IntData.Begin + ((tweenPtr->Data.IntData.End - tweenPtr->Data.IntData.Begin) * progress);
		if (tweenPtr->CurrentTimeSeconds > tweenPtr->TotalTimeSeconds) {
			progressVal = tweenPtr->Data.IntData.End;
		}
		*tweenPtr->Data.IntData.Value = (int)progressVal;
	}
}

Tween CreateFloatTween(float* val, float start, float end, float timeSeconds, TweenEasings ease) {
	Tween tweenId = getFirstUnusedTween();
	sgTween* tweenPtr = &_tweens[tweenId];
	tweenPtr->TweenType = TweenTypeFloat;
	tweenPtr->Data.FloatData.Begin = start;
	tweenPtr->Data.FloatData.End = end;
	tweenPtr->Data.FloatData.Value = val;
	tweenPtr->TotalTimeSeconds = timeSeconds;
	tweenPtr->EaseType = ease;
	return tweenId;
}

Tween CreateIntTween(int* val, int start, int end, float timeSeconds, TweenEasings ease) {
	Tween tweenId = getFirstUnusedTween();
	sgTween* tweenPtr = &_tweens[tweenId];
	tweenPtr->TweenType = TweenTypeInt;
	tweenPtr->Data.IntData.Begin = start;
	tweenPtr->Data.IntData.End = end;
	tweenPtr->Data.IntData.Value = val;
	tweenPtr->TotalTimeSeconds = timeSeconds;
	tweenPtr->EaseType = ease;
	return tweenId;
}

void StartTween(Tween tween) {
	if (tween >= 0 && tween < MAX_TWEENS && !_tweens[tween].Available) {
		_tweens[tween].Started = true;
	}
}

void StopTween(Tween tween) {
	if (tween >= 0 && tween < MAX_TWEENS) {
		_tweens[tween].Started = false;
		_tweens[tween].Available = true;
		_tweens[tween].CurrentTimeSeconds = 0;
	}
}

void SetTweenFunctions(Tween tween, TweenStartFunc start, TweenUpdateFunc update, TweenEndFunc end) {
	if (tween >= 0 && tween < MAX_TWEENS) {
		_tweens[tween].StartFunc = start;
		_tweens[tween].UpdateFunc = update;
		_tweens[tween].EndFunc = end;
	}
}

void SetTweenUserdata(Tween tween, void* userdata) {
	if (tween >= 0 && tween < MAX_TWEENS) {
		_tweens[tween].Userdata = userdata;
	}
}
