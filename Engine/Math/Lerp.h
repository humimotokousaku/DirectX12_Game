#pragma once
#include "MathStructs.h"
#include <vector>

namespace Lerps {
	// 線形補間
	Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
	float Lerp(const float& start, const float& end, float t);
	// 球面線形補間
	Vector3 Slerp(const Vector3& start, const Vector3& end, float t);
	// Cat-mullRom曲線
	Vector3 CatmullRomSpline(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
	Vector3 CatmullRomSpline(const std::vector<Vector3>& controlPoints, float t);

	// 指数補間関数
	Vector3 ExponentialInterpolate(const Vector3& current, const Vector3& target, float damping, float deltaTime);
	float ExponentialInterpolate(const float& current, const float& target, float damping, float deltaTime);
};

namespace Easings {
#pragma region Quint

	// だんだん減速(ぎりぎりまで速度が速い)
	float EaseOutQuint(float x);

	// だんだん早くなる(一気に早くなる)
	float EaseInQuint(float x);

	float EaseInOutQuint(float x);

#pragma endregion

#pragma region Circ

	float EaseInCirc(float x);

	float EaseOutCirc(float x);

	float EaseInOutCirc(float x);

#pragma endregion

#pragma region Cubic

	float EaseInCubic(float x);

	float EaseOutCubic(float x);

	float EaseInOutCubic(float x);

#pragma endregion

#pragma region Back

	float EaseInBack(float x);

	float EaseOutBack(float x);

	float EaseInOutBack(float x);

#pragma endregion

#pragma region Bounce

	float EaseOutBounce(float x);

	float EaseInBounce(float x);

	float EaseInOutBounce(float x);

#pragma endregion

#pragma region Elastic

	float EaseInElastic(float x);

	float EaseOutElastic(float x);

	float EaseInOutElastic(float x);

#pragma endregion

#pragma region Quart

	float EaseInQuart(float x);

	float EaseOutQuart(float x);

	float EaseInOutQuart(float x);

#pragma endregion

#pragma region Expo

	float EaseInExpo(float x);

	float EaseOutExpo(float x);

	float EaseInOutExpo(float x);

#pragma endregion
}