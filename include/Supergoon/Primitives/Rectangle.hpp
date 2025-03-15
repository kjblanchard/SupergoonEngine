#pragma once
#include <SDL3/SDL_rect.h>

namespace Supergoon {
struct Rectangle {
	int X, Y, W, H;
	inline Rectangle() : X(0), Y(0), W(0), H(0) {}
	inline Rectangle(int x, int y, int w, int h) : X(x), Y(y), W(w), H(h) {}
	inline bool Zero() { return (X == 0 && Y == 0 && W == 0 && H == 0); }
	operator SDL_FRect() const {
		return SDL_FRect{(float)X, (float)Y, (float)W, (float)H};
	}
	operator SDL_Rect() const {
		return SDL_Rect{X, Y, W, H};
	}
	inline bool IsOverlap(Rectangle* rhs) {
		return SDL_HasRectIntersection((SDL_Rect*)this, (SDL_Rect*)rhs);
	}
	inline bool IsOverlap(Rectangle& rhs) {
		return SDL_HasRectIntersection((SDL_Rect*)this, (SDL_Rect*)&rhs);
	}
	inline Rectangle GetOverlapRect(Rectangle* rhs) {
		if (!IsOverlap(rhs)) {
			return Rectangle();
		}
		Rectangle r = Rectangle();
		SDL_GetRectIntersection((SDL_Rect*)this, (SDL_Rect*)rhs, (SDL_Rect*)&r);
		return r;
	}
	inline bool operator==(const Rectangle& rhs) {
		return X == rhs.X && Y == rhs.Y && W == rhs.W && H == rhs.H;
	}
};

struct RectangleF {
	float X, Y, W, H;
	inline RectangleF() : X(0), Y(0), W(0), H(0) {}
	inline RectangleF(float x, float y, float w, float h) : X(x), Y(y), W(w), H(h) {}
	inline bool Zero() { return (X == 0 && Y == 0 && W == 0 && H == 0); }
	operator SDL_Rect() const {
		return SDL_Rect{(int)X, (int)Y, (int)W, (int)H};
	}
	operator SDL_FRect() const {
		return SDL_FRect{X, Y, W, H};
	}
	inline bool IsOverlap(RectangleF* rhs) {
		return SDL_HasRectIntersectionFloat((SDL_FRect*)this, (SDL_FRect*)rhs);
	}
	inline RectangleF GetOverlapRect(RectangleF* rhs) {
		if (!IsOverlap(rhs)) {
			RectangleF r = {0, 0, 0, 0};
			return r;
		}
		RectangleF r;
		SDL_GetRectIntersectionFloat((SDL_FRect*)this, (SDL_FRect*)rhs, (SDL_FRect*)&r);
		return r;
	}
	inline bool operator==(const RectangleF& rhs) {
		return X == rhs.X && Y == rhs.Y && W == rhs.W && H == rhs.H;
	}
};

}  // namespace Supergoon
