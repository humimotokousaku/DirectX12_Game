#include "Utility.h"

namespace Utility {
	// 符号を取得する
	float Sign(float value) {
		// -
		if (value < 0) { return -1; }
		// +
		else  if (value > 0) { return 1; }
		// なし
		return 0;
	}
}