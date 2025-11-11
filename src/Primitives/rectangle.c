#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <Supergoon/Primitives/rectangle.h>
#include <math.h>

int RectIsCollision(RectangleF* lhs, RectangleF* rhs) {
	return !(lhs->x + lhs->w < rhs->x ||
			 lhs->x > rhs->x + rhs->w ||
			 lhs->y + lhs->h < rhs->y ||
			 lhs->y > rhs->y + rhs->h);
}

void RectResolveCollision(RectangleF* lhs, RectangleF* rhs) {
	float dx1 = (rhs->x + rhs->w) - lhs->x;	 // left overlap
	float dx2 = (lhs->x + lhs->w) - rhs->x;	 // right overlap
	float dy1 = (rhs->y + rhs->h) - lhs->y;	 // top overlap
	float dy2 = (lhs->y + lhs->h) - rhs->y;	 // bottom overlap
	float xOverlap = dx1 < dx2 ? dx1 : -dx2;
	float yOverlap = dy1 < dy2 ? dy1 : -dy2;
	if (fabsf(xOverlap) < fabsf(yOverlap)) {
		lhs->x += xOverlap;
	} else {
		lhs->y += yOverlap;
	}
}

int RectIsRect0(RectangleF rect) {
	return rect.x == 0 && rect.y == 0 && rect.w == 0 && rect.h == 0;
}
