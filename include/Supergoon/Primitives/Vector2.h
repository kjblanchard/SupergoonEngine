#pragma once
typedef struct sgVector2 {
  float X, Y;
} sgVector2;

inline void sgVector2Multiply(sgVector2 *product, sgVector2 mult) {
  product->X *= mult.X;
  product->Y *= mult.Y;
}
inline void sgVector2Add(sgVector2 *product, sgVector2 add) {
  product->X += add.X;
  product->Y += add.Y;
}

inline bool sgVector2IsEqual(const sgVector2 *lhs, const sgVector2 *rhs) {
  return lhs->X == rhs->X && rhs->Y == rhs->Y;
}
