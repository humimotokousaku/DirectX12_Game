#include "Lerp.h"
#include <cmath>
#include <numbers>
#include <algorithm>
#include <array>
#include <assert.h>

namespace Lerps {
	Vector4 Lerp(const Vector4& start, const Vector4& end, float t) {
		Vector4 result = {
			(1.0f - t) * start.x + t * end.x,
			(1.0f - t) * start.y + t * end.y,
			(1.0f - t) * start.z + t * end.z,
			(1.0f - t) * start.w + t * end.w
		};
		return result;
	}
	Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
		Vector3 result = {
			(1.0f - t) * start.x + t * end.x,
			(1.0f - t) * start.y + t * end.y,
			(1.0f - t) * start.z + t * end.z
		};
		return result;
	}
	Vector2 Lerp(const Vector2& start, const Vector2& end, float t) {
		Vector2 result = {
			(1.0f - t) * start.x + t * end.x,
			(1.0f - t) * start.y + t * end.y
		};
		return result;
	}
	float Lerp(const float& start, const float& end, float t) {
		float result = (1.0f - t) * start + t * end;

		return result;
	}

	Vector3 Slerp(const Vector3& start, const Vector3& end, float t) {
		float dot = Dot(start, end);
		float theta = (float)acos((dot * (float)std::numbers::pi) / 180);
		float sinTheta = (float)sin(theta);
		float weightStart = (float)sin((1 - t) * theta) / sinTheta;
		float weightEnd = (float)sin(t * theta) / sinTheta;

		Vector3 result;
		result.x = (weightStart * start.x + weightEnd * end.x);
		result.y = (weightStart * start.y + weightEnd * end.y);
		result.z = (weightStart * start.z + weightEnd * end.z);
		return result;
	}

	Vector3 CatmullRomSpline(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
		const float s = 0.5f;
		float t2 = t * t;
		float t3 = t2 * t;

		Vector3 e3 = (p0 * -1.0f) + (p1 * 3.0f) - (p2 * 3.0f) + p3;
		Vector3 e2 = (p0 * 2.0f) - (p1 * 5.0f) + (p2 * 4.0f) - p3;
		Vector3 e1 = (p0 * -1.0f) + p2;
		Vector3 e0 = p1 * 2.0f;
		return (e3 * t3 + e2 * t2 + e1 * t + e0) * s;
	}
	Vector3 CatmullRomSpline(const std::vector<Vector3>& controlPoints, float t) {
		assert(controlPoints.size() >= 4 && "制御点は4点未満です");

		// 区画数を制御点の数-1
		size_t division = controlPoints.size() - 1;
		// 1区間の長さ(全体を1)
		float areaWidth = 1.0f / division;

		// 区間内の始点を0 終点を1の時の現在地
		float t_2 = std::fmod(t, areaWidth) * division;
		// 0~1を出ないようにする
		t_2 = std::clamp<float>(t_2, 0.0f, 1.0f);

		// 区間番号
		size_t index = static_cast<size_t>(t / areaWidth);
		// 区間番号が上限を超えないようにする
		index = std::clamp<size_t>(index, 0, controlPoints.size() - 1);

		// 4線分のインデックス
		std::array<size_t, 4> indeces = {
			indeces[0] = index - 1,
			indeces[1] = index,
			indeces[2] = index + 1,
			indeces[3] = index + 2
		};
		// 制御点のサイズを超えないようにする
		indeces[0] = std::clamp<size_t>(indeces[0], 0, controlPoints.size() - 1);
		indeces[1] = std::clamp<size_t>(indeces[1], 0, controlPoints.size() - 1);
		indeces[2] = std::clamp<size_t>(indeces[2], 0, controlPoints.size() - 1);
		indeces[3] = std::clamp<size_t>(indeces[3], 0, controlPoints.size() - 1);

		// 最初の区間のp0はp1を重複使用
		if (index == 0) {
			indeces[0] = indeces[1];
		}
		// 最後の区間のp3はp2を重複使用
		if (indeces[3] >= controlPoints.size()) {
			indeces[2] -= 1;
			indeces[3] = indeces[2];
		}
		// 4点の座標
		std::array<Vector3, 4> points = {
			points[0] = controlPoints[indeces[0]],
			points[1] = controlPoints[indeces[1]],
			points[2] = controlPoints[indeces[2]],
			points[3] = controlPoints[indeces[3]]
		};

		return CatmullRomSpline(points[0], points[1], points[2], points[3], t_2);
	}

	Vector3 ExponentialInterpolate(const Vector3& current, const Vector3& target, float damping) {
		float factor = 1.0f - std::exp(-damping);
		return current + (target - current) * factor;
	}
	Vector2 ExponentialInterpolate(const Vector2& current, const Vector2& target, float damping) {
		float factor = 1.0f - std::exp(-damping);
		return current + (target - current) * factor;
	}
	float ExponentialInterpolate(const float& current, const float& target, float damping) {
		float factor = 1.0f - std::exp(-damping);
		return current + (target - current) * factor;
	}
}

namespace Easings {
	float EaseInSine(float t) {
		return 1 - std::cos((t * (float)std::numbers::pi) / 2.0f);
	}
	float EaseOutSine(float t) {
		return std::sin((t * (float)std::numbers::pi) / 2.0f);
	}
	float EaseInOutSine(float t) {
		return -(std::cos((float)std::numbers::pi * t) - 1.0f) / 2.0f;
	}

#pragma region Cubic
	float EaseInCubic(float x) {
		return x * x * x;
	}

	float EaseOutCubic(float x) {
		return 1 - (float)pow(1 - x, 3);
	}

	float EaseInOutCubic(float x) {
		if (x < 0.5f) {
			return 4 * x * x * x;
		}
		else {
			return 1 - (float)pow(-2 * x + 2, 3) / 2;
		}
	}

#pragma endregion

#pragma region Quint
	// だんだん早くなる(一気に早くなる)
	float EaseInQuint(float x) {
		return x * x * x * x * x;
	}

	// だんだん減速(ぎりぎりまで速度が速い)
	float EaseOutQuint(float x) {
		return 1 - (float)pow(1 - x, 5);
	}

	// EaseInOut(一気に早くなって減速する)
	float EaseInOutQuint(float x) {
		if (x < 0.5) {
			return 16 * x * x * x * x * x;
		}
		else {
			return 1 - (float)pow(-2 * x + 2, 5) / 2;
		}
	}

#pragma endregion

#pragma region Circ

	float EaseInCirc(float x) {
		return 1 - (float)sqrt(1 - (float)pow(x, 2));
	}

	float EaseOutCirc(float x) {
		return (float)sqrt(1 - (float)pow(x - 1, 2));
	}

	float EaseInOutCirc(float x) {
		if (x < 0.5f) {
			return (1 - (float)sqrt(1 - (float)pow(2 * x, 2))) / 2;
		}
		else {
			return ((float)sqrt(1 - (float)pow(-2 * x + 2, 2)) + 1) / 2;
		}
	}

#pragma endregion

#pragma region Back

	float EaseInBack(float x) {
		const float c1 = 1.70158f;
		const float c3 = c1 + 1;

		return c3 * x * x * x - c1 * x * x;
	}

	float EaseOutBack(float x) {
		const float c1 = 1.70158f;
		const float c3 = c1 + 1;

		return 1 + c3 * (float)pow(x - 1, 3) + c1 * (float)pow(x - 1, 2);
	}

	float EaseInOutBack(float x) {
		const float c1 = 1.70158f;
		const float c2 = c1 * 1.525f;

		if (x < 0.5f) {
			return ((float)pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2;
		}
		else {
			return ((float)pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
		}
	}

#pragma endregion

#pragma region Bounce

	float EaseOutBounce(float x) {
		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		if (x < 1 / d1) {
			return n1 * x * x;
		}
		else if (x < 2 / d1) {
			return n1 * (x -= 1.5f / d1) * x + 0.75f;
		}
		else if (x < 2.5 / d1) {
			return n1 * (x -= 2.25f / d1) * x + 0.9375f;
		}
		else {
			return n1 * (x -= 2.625f / d1) * x + 0.984375f;
		}
	}

	float EaseInBounce(float x) {
		return 1 - EaseOutBounce(1 - x);
	}

	float EaseInOutBounce(float x) {
		if (x < 0.5f) {
			return (1 - EaseOutBounce(1 - 2 * x)) / 2;
		}
		else {
			return (1 + EaseOutBounce(2 * x - 1)) / 2;
		}
	}

#pragma endregion

#pragma region Elastic

	float EaseInElastic(float x) {
		const float c4 = (2 * (float)std::numbers::pi) / 3;
		if (x == 0) {
			return x = 0;
		}
		else {
			if (x == 1) {
				return x = 1;
			}
			else {
				return -(float)pow(2, 10 * x - 10) * (float)sin((x * 10 - 10.75) * c4);
			}
		}
	}

	float EaseOutElastic(float x) {
		const float c4 = (2 * (float)std::numbers::pi) / 3;
		if (x == 0) {
			return x = 0;
		}
		else {
			if (x == 1) {
				return x = 1;
			}
			else {
				return (float)pow(2, -10 * x) * (float)sin((x * 10 - 0.75f) * c4) + 1;
			}
		}
	}

	float EaseInOutElastic(float x) {
		const float c5 = (2 * (float)std::numbers::pi) / 4.5f;
		if (x == 0) {
			return x = 0;
		}
		else {
			if (x == 1) {
				return x = 1;
			}
			else {
				if (x < 0.5f) {
					return -((float)pow(2, 20 * x - 10) * (float)sin((20 * x - 11.125f) * c5)) / 2;
				}
				else {
					return ((float)pow(2, -20 * x + 10) * (float)sin((20 * x - 11.125f) * c5)) / 2 + 1;
				}
			}
		}
	}

#pragma endregion

#pragma region Quart

	float EaseInQuart(float x) {
		return x * x * x * x;
	}

	float EaseOutQuart(float x) {
		return 1 - (float)pow(1 - x, 4);
	}

	float EaseInOutQuart(float x) {
		if (x < 0.5f) {
			return 8 * x * x * x * x;
		}
		else {
			return 1 - (float)pow(-2 * x + 2, 4) / 2;
		}
	}

#pragma endregion

#pragma region Expo

	float EaseInExpo(float x) {
		if (x == 0) {
			return x = 0;
		}
		else {
			return (float)pow(2, 10 * x - 10);
		}
	}

	float EaseOutExpo(float x) {
		if (x == 1) {
			return x = 1;
		}
		else {
			return 1 - (float)pow(2, -10 * x);
		}
	}

	float EaseInOutExpo(float x) {
		if (x == 0) {
			return x = 0;
		}
		else {
			if (x == 1) {
				return x = 1;
			}
			else {
				if (x < 0.5f) {
					return (float)pow(2, 20 * x - 10) / 2;
				}
				else {
					return (2 - (float)pow(2, -20 * x + 10)) / 2;
				}
			}
		}
	}

#pragma endregion
}