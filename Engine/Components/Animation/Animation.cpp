#include "Animation.h"
#include "WinApp.h"
#include "Lerp.h"
#include <algorithm>

Animation::Animation() {
	// ゲームタイマーのインスタンス取得
	gameTimer_ = GameTimer::GetInstance();
	isStart_ = false;
	animId_ = 0;
}

void Animation::Update() {
	if (isStart_) {
		for (std::list<AnimData>::iterator it = animData_.begin(); it != animData_.end(); ++it) {
			if (it->isActive) {
				// アニメーション終了
				if (it->currentFrame > it->endFrame) {
					it->isActive = false;
					// 次のアニメーションがあるか
					if (std::next(it) != animData_.end()) {
						// 次のアニメーションを起動
						std::next(it)->isActive = true;
					}
					else if (std::next(it) == animData_.end()) {
						isStart_ = false;
						isEnd_ = true;
						break;
					}
				}
				else {
					isEnd_ = false;

					// アニメーション
					it->t = it->easeFunc((float)it->currentFrame / it->endFrame);

					// イージング処理
					std::visit([&](auto&& targetPtr) {
						// 引き数の型を取得して同じ型の変数を作成
						using T = std::decay_t<decltype(*targetPtr)>;
						// 同じ型かを検出
						if constexpr (std::is_same_v<T, Vector4>) {
							*targetPtr = Lerps::Lerp(std::get<Vector4>(it->start), std::get<Vector4>(it->end), it->t);
						}
						else if constexpr (std::is_same_v<T, Vector3>) {
							*targetPtr = Lerps::Lerp(std::get<Vector3>(it->start), std::get<Vector3>(it->end), it->t);
						}
						else if constexpr (std::is_same_v<T, Vector2>) {
							*targetPtr = Lerps::Lerp(std::get<Vector2>(it->start), std::get<Vector2>(it->end), it->t);
						}
						else if constexpr (std::is_same_v<T, float>) {
							*targetPtr = Lerps::Lerp(std::get<float>(it->start), std::get<float>(it->end), it->t);
						}
						else if constexpr (std::is_same_v<T, int>) {
							*targetPtr = (int)Lerps::Lerp((float)std::get<int>(it->start), (float)std::get<int>(it->end), it->t);
						}
						}, (*it).target);
					it->currentFrame += gameTimer_->GetTimeScale();

					// アニメーション時間の上限を超えないようにする
					//it->currentFrame = std::clamp<float>(it->currentFrame, 0, it->endFrame);
				}
			}
		}
	}
	else if (!isStart_) {
		// 全てのデータを初期化
		for (std::list<AnimData>::iterator it = animData_.begin(); it != animData_.end(); ++it) {
			it->t = 0;
			it->currentFrame = 0;
			it->isActive = false;
		}
	}
}

void Animation::SetAnimData(std::variant<Vector4*, Vector3*, Vector2*, float*, int*> target, std::variant<Vector4, Vector3, Vector2, float, int> start, std::variant<Vector4, Vector3, Vector2, float, int> end, float endFrame, std::function<float(float)> easeFunc) {
	AnimData animData = {
		target,
		0,
		0,
		endFrame,
		0,
		start,
		end,
		false,
		animId_,
		easeFunc
	};
	// 最初のアニメーションは起動させておく
	if (animData.id == 0) {
		animData.isActive = true;
	}

	// リストが空なら登録
	animData_.push_back(animData);

	// アニメーションのIDをインクリメント
	animId_++;
}

void Animation::SetFirstAnimData(std::variant<Vector4*, Vector3*, Vector2*, float*, int*> target, std::variant<Vector4, Vector3, Vector2, float, int> start, std::variant<Vector4, Vector3, Vector2, float, int> end, float endFrame, std::function<float(float)> easeFunc) {
	AnimData animData = {
	target,
	0,
	0,
	endFrame,
	0,
	start,
	end,
	true,
	0,
	easeFunc
	};

	// リストが空なら登録
	if (animData_.size() == 0) {
		animData_.push_back(animData);
		animId_++;
		return;
	}
	animData_.front() = animData;
}

void Animation::AnimDataOverride(int index, std::variant<Vector4*, Vector3*, Vector2*, float*, int*> target, std::variant<Vector4, Vector3, Vector2, float, int> start, std::variant<Vector4, Vector3, Vector2, float, int> end, float endFrame, std::function<float(float)> easeFunc) {
	AnimData animData = {
	target,
	0,
	0,
	endFrame,
	0,
	start,
	end,
	false,
	index,
	easeFunc
	};

	// イテレータをリストの先頭に設定
	std::list<AnimData>::iterator it = animData_.begin();

	// index分要素に進む
	std::advance(it, index);
	*it = animData;
}