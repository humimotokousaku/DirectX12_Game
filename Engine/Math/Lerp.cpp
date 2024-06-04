#include "Lerp.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace Lerps {

	Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
		float newX = (1.0f - t) * start.x + t * end.x;
		float newY = (1.0f - t) * start.y + t * end.y;
		float newZ = (1.0f - t) * start.z + t * end.z;

		return Vector3(newX, newY, newZ);
	}

	Vector3 Slerp(const Vector3& start, const Vector3& end, float t) {
		float dot = Dot(start, end);
		float theta = (float)acos((dot * (float)(float)M_PI) / 180);
		float sinTheta = (float)sin(theta);
		float weightStart = (float)sin((1 - t) * theta) / sinTheta;
		float weightEnd = (float)sin(t * theta) / sinTheta;

		Vector3 result;
		result.x = (weightStart * start.x + weightEnd * end.x);
		result.y = (weightStart * start.y + weightEnd * end.y);
		result.z = (weightStart * start.z + weightEnd * end.z);
		return result;
	}

	Vector3 CatmullRomSpline(const std::vector<Vector3>& controlPoints, float t) {
		int n = (int)controlPoints.size();
		int segment = static_cast<int>(t * (n - 1));
		float tSegment = t * (n - 1) - segment;

		Vector3 p0 = controlPoints[segment > 0 ? segment - 1 : 0];
		Vector3 p1 = controlPoints[segment];
		Vector3 p2 = controlPoints[segment < n - 1 ? segment + 1 : n - 1];
		Vector3 p3 = controlPoints[segment < n - 2 ? segment + 2 : n - 1];

		Vector3 interpolatedPoint;
		interpolatedPoint.x =
			0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * tSegment +
				(2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * (tSegment * tSegment) +
				(-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * (tSegment * tSegment * tSegment));
		interpolatedPoint.y =
			0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * tSegment +
				(2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * (tSegment * tSegment) +
				(-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * (tSegment * tSegment * tSegment));
		interpolatedPoint.z =
			0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * tSegment +
				(2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * (tSegment * tSegment) +
				(-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * (tSegment * tSegment * tSegment));

		return interpolatedPoint;
	}
}

namespace Easings {
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
		const float c4 = (2 * (float)M_PI) / 3;
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
		const float c4 = (2 * (float)M_PI) / 3;
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
		const float c5 = (2 * (float)M_PI) / 4.5f;
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