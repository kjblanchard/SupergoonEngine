/**
 * @file nineslice.h
 * @brief nineslice ui object
 * @author Kevin Blanchard
 * @version 0.1.0
 * @date 2026-09-29
 */
#pragma once
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/Point.h>
#include <Supergoon/UI/object.h>
#ifdef __cplusplus
extern "C" {
#endif

struct Texture;

typedef struct UINineSliceData {
	struct Texture* Texture;
	RectangleF SourceRect;
} UINineSliceData;

typedef struct UINineSliceArgs {
	UIObject* Object;
	struct Texture* Texture;
	Color Color;
	Point NineSliceOffset;
} UINineSliceArgs;

UIObject* CreateUINineSlice(UINineSliceArgs* args);

#ifdef __cplusplus
}
#endif
