#include "Utility.h"

namespace Utility {
	// 符号を取得する
	float Sign(float value) {
		// -
		if (value < 0.0f) { return -1.0f; }
		// +
		else  if (value > 0.0f) { return 1.0f; }
		// なし
		return 0;
	}
}