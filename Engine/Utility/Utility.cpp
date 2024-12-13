#include "Utility.h"

namespace Utility {
	float Sign(float value) {
		// -
		if (value < 0.0f) { return -1.0f; }
		// +
		else  if (value > 0.0f) { return 1.0f; }
		// なし
		return 0;
	}

	Vector3 Custom_fmod(Vector3 dividend, float divisor, float initValue) {
		Vector3 result = {
			Custom_fmod(dividend.x, divisor, initValue),
			Custom_fmod(dividend.y, divisor, initValue),
			Custom_fmod(dividend.z, divisor, initValue)
		};

		return result;
	}

	float Custom_fmod(float dividend, float divisor, float initValue) {
		float result;

		// 既定数値以上になったら指定した値に初期化
		// divisorが-の場合
		if (dividend < 0.0f || dividend >= divisor) {
			result = initValue;
			return result;
		}

		result = dividend;
		return result;
	}
}