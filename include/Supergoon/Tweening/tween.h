/**
 * @file tween.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief  inbeTWEEN system for the engine, handling all living tweens, and able to create them.
 * @version 0.1
 * @date 2025-03-17
 * @copyright Copyright (c) 2025
 */
#pragma once
// The Easing types used when creating a tween.
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
// Tween Flags, currently unused.
typedef enum TweenFlags {
	TweenFlagDefault,
	TweenFlagPingPong,
} TweenFlags;
// A handle to a tween
typedef unsigned int Tween;
// Will be called once when tween is started, userdata will be passed if set
typedef void (*TweenStartFunc)(void*);
// Will be called every frame when tween is started, userdata will be passed if set
typedef void (*TweenUpdateFunc)(void*);
// Will be called once when tween is ended, userdata will be passed if set
typedef void (*TweenEndFunc)(void*);
/**
 * @brief Create a Float Tween object
 *
 * @param val Ptr to value to modify
 * @param start starting value
 * @param end  ending value
 * @param timeSeconds length of tween
 * @param ease Type of easing to use
 * @return Tween handle to tween
 */
Tween CreateFloatTween(float* val, float start, float end, float timeSeconds, TweenEasings ease);
/**
 * @brief Create a Int Tween object
 *
 * @param val Ptr to value to modify
 * @param start starting value
 * @param end  ending value
 * @param timeSeconds length of tween
 * @param ease Type of easing to use
 * @return Tween handle to tween
 */
Tween CreateIntTween(int* val, int start, int end, float timeSeconds, TweenEasings ease);
/**
 * @brief Starts a tween that has already been created with CreateXTween
 *
 * @param tween tween handle
 */
void StartTween(Tween tween);
/**
 * @brief Pauses a tween that has already been created with CreateXTween
 *
 * @param tween tween handle
 */
void PauseTween(Tween tween);
/**
 * @brief Stops a tween that has already been created with CreateXTween
 *
 * @param tween tween handle
 */
void StopTween(Tween tween);
// Sets the functions to be called on a tween that has already been created with CreateXTween
void SetTweenFunctions(Tween tween, TweenStartFunc start, TweenUpdateFunc update, TweenEndFunc end);
// Sets the userdata to be passed to update funcs on a tween that has already been created with CreateXTween
void SetTweenUserdata(Tween tween, void* userdata);
