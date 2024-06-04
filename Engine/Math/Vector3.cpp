#include "Vector3.h"
#include <cmath>

Vector3 Add(const Vector3& pos, const Vector3& vector) {
	Vector3 result{};
	result.x = pos.x + vector.x;
	result.y = pos.y + vector.y;
	result.z = pos.z + vector.z;

	return result;
}

Vector3 Subtract(const Vector3& pos1, const Vector3& pos2) {
	Vector3 result{};
	result.x = pos1.x - pos2.x;
	result.y = pos1.y - pos2.y;
	result.z = pos1.z - pos2.z;

	return result;
}

Vector3 Multiply(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return result;
}

Vector3 Multiply(const float& scalar, const Vector3& v) {
	Vector3 result;
	result.x = v.x * scalar;
	result.y = v.y * scalar;
	result.z = v.z * scalar;
	return result;
}

float Dot(const Vector3& v1, const Vector3& v2) {
	float result{};
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

float Length(const Vector3& v) {
	float result{};
	result = sqrt(Dot(v, v));
	return result;
}

Vector3 Normalize(const Vector3& v) {
	Vector3 result{};
	float length = Length(v);
	if (length != 0.0f) {
		result.x = v.x / length;
		result.y = v.y / length;
		result.z = v.z / length;
	}
	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return { v1.y * v2.z - v1.z * v2.y,v1.z * v2.x - v1.x * v2.z,v1.x * v2.y - v1.y * v2.x };
}

float Degree2Radian(float degree) {
	float result = degree * (float)std::numbers::pi / 180;
	return result;
}

float Radian2Degree(float radian) {
	float result = radian * 180 / (float)std::numbers::pi;
	return result;
}