#pragma once
#include <functional>

class TimedCall {
public: // メンバ変数
	// コンストラクタ
	TimedCall(std::function<void()> f, uint32_t time);

	// 更新処理
	void Update();

	// 完了ならtrueを返す
	bool IsFinished()const{ return isFinished_; }

private:
	// コールバック
	std::function<void()> f_;
	// 残り時間
	uint32_t time_;
	// 完了フラグ
	bool isFinished_ = false;
};
