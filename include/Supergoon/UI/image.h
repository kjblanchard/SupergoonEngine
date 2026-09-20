/**
 * @file image.h
 * @brief UI image object
 * @author Kevin Blanchard
 * @version 0.1.0
 * @date 2026-09-19
 */
#pragma once
#include <Supergoon/UI/object.h>
#ifdef __cplusplus
extern "C" {
#endif

struct Sprite;

typedef struct UIImageData {
	struct Sprite* Sprite;
} UIImageData;

typedef struct UIImageArgs {
	UIObject* Object;
	struct Sprite* Sprite;
} UIImageArgs;

UIObject* CreateUIImage(UIImageArgs* args);

#ifdef __cplusplus
}
#endif
