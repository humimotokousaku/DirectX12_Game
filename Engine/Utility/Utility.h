#pragma once
#include "Vector3.h"

namespace Utility {
	// 符号を取得する
	float Sign(float value);

	Vector3 Custom_fmod(Vector3 dividend, float divisor, float initValue);
	/// <summary>
	/// std::fmodを使用して指定した値に初期化
	/// </summary>
	/// <param name="dividend">剰余を求めたい値</param>
	/// <param name="divisor">割る数</param>
	/// <param name="initValue">剰余が0のときの値</param>
	/// <returns></returns>
	float Custom_fmod(float dividend, float divisor, float initValue);
}
