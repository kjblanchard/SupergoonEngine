#include <SDL3/SDL_events.h>
#include <Supergoon/Tweening/easing.h>
#include <Supergoon/Tweening/tween.h>
#include <Supergoon/events.h>
#include <Supergoon/log.h>
#include <Supergoon/state.h>
#include <SupergoonEngine/tween.h>
#include <stdbool.h>
#include <stdlib.h>

static void updateTween(sgTween* tweenPtr, Tween id);
static void startTween(Tween tween);
static void pauseTween(Tween tween);
static void stopTween(Tween tween);
static void zeroTween(Tween tween);
static void updateTweenData(sgTween* tweenPtr);
// Tweens allocated by the engine
sgTween _tweens[MAX_TWEENS];
// Current number of tweens, used when iterating to reduce iteration if possible.
int _numberTweens = 0;
/**
 * @brief Initializes the tween engine for use, called by engine.
 *
 */
void initializeTweenEngine(void) {
	for (size_t i = 0; i < MAX_TWEENS; i++) {
		zeroTween(i);
	}
}
/**
 * @brief Handles the tween events, called by event engine
 *
 * @param event
 */
void handleTweenEvents(Event* event) {
	if (!(event->type == BuiltinEventIds.StopTweenEvent ||
		  event->type == BuiltinEventIds.StartTweenEvent ||
		  event->type == BuiltinEventIds.PauseTweenEvent)) return;
	Tween tweenId = event->user.code;
	if (event->type == BuiltinEventIds.StartTweenEvent) {
		startTween(tweenId);
	} else if (event->type == BuiltinEventIds.PauseTweenEvent) {
		pauseTween(tweenId);
	} else if (event->type == BuiltinEventIds.StopTweenEvent) {
		stopTween(tweenId);
	}
}

static bool isTweenAvailableForActions(Tween tween) {
	if (_tweens[tween].Available) {
		sgLogInfo("Trying to perform actions on a tween that isn't available");
		return false;
	}
	return true;
}

static void startTween(Tween tween) {
	if (!isTweenAvailableForActions(tween)) return;
	_tweens[tween].Started = true;
}

static void pauseTween(Tween tween) {
	if (!isTweenAvailableForActions(tween)) return;
	_tweens[tween].Started = false;
}

static void stopTween(Tween tween) {
	if (!isTweenAvailableForActions(tween)) return;
	_tweens[tween].Started = false;
	_tweens[tween].Available = true;
	_tweens[tween].CurrentTimeSeconds = 0;
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
	if (tweenPtr->CurrentTimeSeconds == 0 && tweenPtr->StartFunc) {
		tweenPtr->StartFunc(tweenPtr->Userdata);
	}
	tweenPtr->CurrentTimeSeconds += DeltaTimeSeconds;
	if (tweenPtr->UpdateFunc) {
		tweenPtr->UpdateFunc(tweenPtr->Userdata);
	}
	updateTweenData(tweenPtr);
	if (tweenPtr->CurrentTimeSeconds >= tweenPtr->TotalTimeSeconds) {
		// If we should reverse, reverse it
		// If we should loop, do it again
		if (tweenPtr->EndFunc) {
			tweenPtr->EndFunc(tweenPtr->Userdata);
		}
		PushEvent(BuiltinEventIds.StopTweenEvent, id, NULL, NULL);
	}
}

static void updateTweenData(sgTween* tweenPtr) {
	float progress = 0;
	switch (tweenPtr->EaseType) {
		case TweenEasingLinear:
			// progress = geLinearInterpolation(_currentDuration / _duration);
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
		*tweenPtr->Data.IntData.Value = progressVal;
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
	tweenPtr->TweenType = TweenTypeFloat;
	tweenPtr->Data.IntData.Begin = start;
	tweenPtr->Data.IntData.End = end;
	tweenPtr->Data.IntData.Value = val;
	tweenPtr->TotalTimeSeconds = timeSeconds;
	tweenPtr->EaseType = ease;
	return tweenId;
}

void StartTween(Tween tween) {
	PushEvent(BuiltinEventIds.StartTweenEvent, tween, NULL, NULL);
}

void StopTween(Tween tween) {
	PushEvent(BuiltinEventIds.StopTweenEvent, tween, NULL, NULL);
}

void SetTweenFunctions(Tween tween, TweenStartFunc start, TweenUpdateFunc update, TweenEndFunc end) {
	_tweens[tween].StartFunc = start;
	_tweens[tween].UpdateFunc = update;
	_tweens[tween].EndFunc = end;
}

void SetTweenUserdata(Tween tween, void* userdata) {
	_tweens[tween].Userdata = userdata;
}
