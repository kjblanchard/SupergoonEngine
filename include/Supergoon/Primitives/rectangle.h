#pragma once
#include <cglm/aabb2d.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct RectangleF {
	float x, y, w, h;

} RectangleF;

int RectIsRect0(RectangleF rect);
int RectIsCollision(RectangleF* lhs, RectangleF* rhs);
void RectResolveCollision(RectangleF* lhs, RectangleF* rhs);
#ifdef __cplusplus
}
#endif
