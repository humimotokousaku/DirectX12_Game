#include "Vector2.h"

float Dot(Vector2 v1, Vector2 v2) {
	float result{};
	result = v1.x * v2.x + v1.y * v2.y;
	return result;
}

float Length(Vector2 v) {
	float result{};
	result = sqrtf(Dot(v, v));
	return result;
}

Vector2 Normalize(const Vector2& v) {
	Vector2 result{};
	float length = Length(v);
	if (length != 0.0f) {
		result.x = v.x / length;
		result.y = v.y / length;
	}
	return result;
}
