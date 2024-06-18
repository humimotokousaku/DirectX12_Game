#include "TimedCall.h"

TimedCall::TimedCall(std::function<void()> f, uint32_t time) {
	f_ = f;
	time_ = time;
}

void TimedCall::Update() {
	if (isFinished_) {
		return;
	}
	time_--;
	if (time_ <= 0) {
		isFinished_ = true;
		// コールバック関数呼び出し
		f_();
	}
}