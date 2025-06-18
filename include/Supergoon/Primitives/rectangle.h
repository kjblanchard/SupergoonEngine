#pragma once
#include <SDL3/SDL_rect.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_FRect RectangleF;
typedef struct SDL_Rect Rectangle;

void ResolveCollision(RectangleF* lhs, RectangleF* rhs);
#ifdef __cplusplus
}
#endif
