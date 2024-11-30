#include "Shake.h"
#include "ImGuiManager.h"

void Shake::Update() {
	if (!isActive_ || isEnd_) { return; }

	*target_ = originalValue_;

	// 範囲を基に値をランダムに揺らす
	value_ = GenerateRandomShake(range_);

	// 補正値
	offset_ = value_ * -1.0f;

	// 対象の値に揺れを加える
	*target_ += value_ * GameTimer::GetInstance()->GetTimeScale();

	// 対象が揺れていないときの値を更新
	originalValue_ = *target_ + offset_ * GameTimer::GetInstance()->GetTimeScale();

	// 時間が過ぎたら終了
	if (currentFrame_ <= 0) {
		currentFrame_ = endFrame_;
		*target_ = originalValue_;
		isActive_ = false;
		isEnd_ = true;
		return;
	}

	currentFrame_--;
}

void Shake::Reset() {
	currentFrame_ = endFrame_;
	isActive_ = false;
	isEnd_ = false;
}

void Shake::ImGuiParameter() {
	if (ImGui::TreeNode("Shake")) {
		ImGui::DragFloat("EndFrame", &endFrame_, 1, 0, 100);
		ImGui::DragFloat3("Range", &range_.x, 0.01f, 0, 100);
		ImGui::TreePop();
	}
}