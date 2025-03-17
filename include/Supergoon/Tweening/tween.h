/**
 * @file tween.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief  inbeTWEEN system for the engine, handling all living tweens, and able to create them.
 * @version 0.1
 * @date 2025-03-17
 * @copyright Copyright (c) 2025
 */
#pragma once
typedef enum TweenEasings {
	TweenEasingLinear,
	// TweenEasingQuadEaseIn,
	// TweenEasingQuadEaseOut,
	// TweenEasingQuadEaseInOut,
	// TweenEasingCubicEaseIn,
	// TweenEasingCubicEaseOut,
	// TweenEasingCubicEaseInOut,
	// TweenEasingQuarticEaseIn,
	// TweenEasingQuarticEaseOut,
	// TweenEasingQuarticEaseInOut,
	// TweenEasingQuinticEaseIn,
	// TweenEasingQuinticEaseOut,
	// TweenEasingQuinticEaseInOut,
	// TweenEasingSineEaseIn,
	// TweenEasingSineEaseOut,
	// TweenEasingSineEaseInOut,
	// TweenEasingCircularEaseIn,
	// TweenEasingCircularEaseOut,
	// TweenEasingCircularEaseInOut,
	// TweenEasingExponentialEaseIn,
	// TweenEasingExponentialEaseOut,
	// TweenEasingExponentialEaseInOut,
	// TweenEasingElasticEaseIn,
	// TweenEasingElasticEaseOut,
	// TweenEasingElasticEaseInOut,
	// TweenEasingBackEaseIn,
	// TweenEasingBackEaseOut,
	// TweenEasingBackEaseInOut,
	// TweenEasingBounceEaseIn,
	// TweenEasingBounceEaseOut,
	// TweenEasingBounceEaseInOut,
} TweenEasings;

typedef enum TweenFlags {
	TweenFlagDefault,
	TweenFlagPingPong,
} TweenFlags;

typedef unsigned int Tween;
typedef void (*TweenStartFunc)(void*);
typedef void (*TweenUpdateFunc)(void*);
typedef void (*TweenEndFunc)(void*);
Tween CreateFloatTween(float* val, float start, float end, float timeSeconds, TweenEasings ease);
Tween CreateIntTween(int* val, int start, int end, float timeSeconds, TweenEasings ease);
void StartTween(Tween tween);
void PauseTween(Tween tween);
void StopTween(Tween tween);
void SetTweenFunctions(Tween tween, TweenStartFunc start, TweenUpdateFunc update, TweenEndFunc end);
void SetTweenUserdata(Tween tween, void* userdata);
