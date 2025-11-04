#include <Supergoon/Tweening/tween.h>
#include <Supergoon/effects.h>
// #include <Supergoon/graphics.h>
#include <Supergoon/log.h>
#include <Supergoon/window.h>
#include <stdbool.h>
#include <stdint.h>

// used im game.cpp
Tween _screenFadeTween = NULL_TWEEN;
int _screenFadeInt = 255;
static bool _currentlyFading = false;
static void fadeScreenStartFunc(void* userdata) {
	_currentlyFading = true;
}
static void fadeScreenUpdateFunc(void* userdata) {
	// SetTextureAlpha(_fullScreenTexture, _screenFadeInt);
}
static void fadeScreenEndFunc(void* userdata) {
	_currentlyFading = false;
}
static void fadeScreen(bool fadeIn, float fadeoutTimeInSeconds) {
	if (_currentlyFading) {
		StopTween(_screenFadeTween);
		_screenFadeTween = NULL_TWEEN;
	}
	int startValue = fadeIn ? 0 : 255;
	int endValue = fadeIn ? 255 : 0;
	_screenFadeTween = CreateIntTween(&_screenFadeInt, startValue, endValue, fadeoutTimeInSeconds, TweenEasingLinear);
	SetTweenFunctions(_screenFadeTween, fadeScreenStartFunc, fadeScreenUpdateFunc, fadeScreenEndFunc);
	StartTween(_screenFadeTween);
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
