#pragma once
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Vector2 {
	float X, Y;
} Vector2;

static inline Vector2 Vector2Multiply(Vector2 lhs, Vector2 rhs) {
	return (Vector2){lhs.X * rhs.X, lhs.Y * rhs.Y};
}

static inline Vector2 Vector2Add(Vector2 lhs, Vector2 rhs) {
	return (Vector2){lhs.X + rhs.X, lhs.Y + rhs.Y};
}

static inline Vector2 Vector2Subtract(Vector2 lhs, Vector2 rhs) {
	return (Vector2){lhs.X - rhs.X, lhs.Y - rhs.Y};
}

static inline bool Vector2IsEqual(const Vector2* lhs, const Vector2* rhs) {
	return lhs->X == rhs->X && lhs->Y == rhs->Y;
}
#ifdef __cplusplus
}
#endif
