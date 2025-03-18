/**
 * @file tween.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief Definition for the actual tween, used by engine and debug
 * @version 0.1
 * @date 2025-03-17
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <Supergoon/Tweening/tween.h>
#include <stdint.h>
#define MAX_TWEENS 100
typedef enum TweenType {
	TweenTypeFloat,
	TweenTypeInt,

} TweenType;

typedef union TweenData {
	struct {
		float Begin, End;
		float* Value;
	} FloatData;
	struct {
		int Begin, End;
		int* Value;
	} IntData;
} TweenData;

typedef struct sgTween {
	TweenType TweenType;
	int Available;
	int Started;
	TweenEasings EaseType;
	int Loops;
	uint8_t Flags;
	float TotalTimeSeconds;
	float CurrentTimeSeconds;
	TweenStartFunc StartFunc;
	TweenUpdateFunc UpdateFunc;
	TweenEndFunc EndFunc;
	TweenData Data;
	void* Userdata;
} sgTween;
