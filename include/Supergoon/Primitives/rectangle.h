#pragma once
#include <cglm/aabb2d.h>
#ifdef __cplusplus
extern "C" {
#endif

// typedef struct Rectangle {
// 	int x, y, w, h;
// } Rectangle;

typedef struct RectangleF {
	float x, y, w, h;

} RectangleF;

int RectIsRect0(RectangleF rect);
void RectResolveCollision(RectangleF* lhs, RectangleF* rhs);
#ifdef __cplusplus
}
#endif
