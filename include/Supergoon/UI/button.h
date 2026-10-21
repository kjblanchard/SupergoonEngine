/**
 * @file button.h
 * @brief UI button, hit area that tracks mouse overlap each tick
 * @author Kevin Blanchard
 * @version 0.1.0
 * @date 2026-10-21
 */
#pragma once
#include <Supergoon/UI/object.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UIButtonMouseStatus {
	UIButtonMouseStatusNone,
	// First tick the mouse is over the button
	UIButtonMouseStatusJustOverlap,
	UIButtonMouseStatusOverlap,
	// First tick after the mouse left the button
	UIButtonMouseStatusJustLeftOverlap,
} UIButtonMouseStatus;

typedef struct UIButtonData {
	unsigned int ButtonActive : 1;
	UIButtonMouseStatus CurrentMouseStatus;
} UIButtonData;

typedef struct UIButtonArgs {
	UIObject* Object;
} UIButtonArgs;

UIObject* CreateUIButton(UIButtonArgs* args);
void UIButtonSetActive(UIObject* o, int active);
UIButtonMouseStatus UIButtonGetMouseStatus(UIObject* o);

#ifdef __cplusplus
}
#endif
