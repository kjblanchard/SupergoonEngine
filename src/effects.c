#include <Supergoon/Tweening/tween.h>
#include <Supergoon/effects.h>
#include <Supergoon/graphics.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/window.h>
#include <stdbool.h>
#include <stdint.h>

// used im game.cpp
int _screenFadeInt = 255;
static bool _currentlyFading = false;
static void fadeScreenStartFunc(void* userdata) {
	_currentlyFading = true;
}
static void fadeScreenUpdateFunc(void* userdata) {
	SetTextureAlpha(_imguiGameTexture, _screenFadeInt);
}
static void fadeScreenEndFunc(void* userdata) {
	_currentlyFading = false;
}
static void fadeScreen(bool fadeIn, float fadeoutTimeInSeconds) {
	if (_currentlyFading) {
		sgLogWarn("Trying to fade but we are already fading, doing nothing");
		return;
	}
	int startValue = fadeIn ? 0 : 255;
	int endValue = fadeIn ? 255 : 0;
	Tween tween = CreateIntTween(&_screenFadeInt, startValue, endValue, fadeoutTimeInSeconds, TweenEasingLinear);
	SetTweenFunctions(tween, fadeScreenStartFunc, fadeScreenUpdateFunc, fadeScreenEndFunc);
	StartTween(tween);
}

int IsScreenFading(void) {
	return _currentlyFading;
}

void FadeinScreen(float fadeinTimeInSeconds) {
	fadeScreen(true, fadeinTimeInSeconds);
}

void FadeoutScreen(float fadeoutTimeInSeconds) {
	fadeScreen(false, fadeoutTimeInSeconds);
}
