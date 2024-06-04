#include "Animation.h"
#include "WinApp.h"
#include "Lerp.h"

Animation::Animation() {
	isStart_ = false;
	animId_ = 0;
}

void Animation::Update() {
	if (isStart_) {
		for (std::list<AnimData>::iterator it = animData_.begin(); it != animData_.end(); ++it) {
			if (it->isActive) {
				it->currentFrame++;
				// アニメーション
				it->t = it->easeFunc((float)it->currentFrame / it->endFrame);
				*it->target = Lerps::Lerp(it->start, it->end, it->t);

				// アニメーション終了
				if (it->t > 1.0f) {
					it->isActive = false;
					it->t = 0;
					it->currentFrame = 0;
					// 次のアニメーションがあるか
					if (std::next(it) != animData_.end()) {
						// 次のアニメーションを起動
						std::next(it)->isActive = true;
					}
					else if (std::next(it) == animData_.end()) {
						isStart_ = false;
					}
				}
			}
		}
	}
	else {
		for (std::list<AnimData>::iterator it = animData_.begin(); it != animData_.end(); ++it) {
			// 最初のアニメーションを常に起動させておく
			if (it->id == 0) {
				it->isActive = true;
			}
		}
	}
}

void Animation::SetAnimData(Vector3* target, Vector3 start, Vector3 end, uint32_t endFrame, const char* name, std::function<float(float)> easeFunc) {
	AnimData animData = {
		target,
		0,
		endFrame,
		0,
		start,
		end,
		false,
		name,
		animId_,
		easeFunc
	};
	// 最初のアニメーションは起動させておく
	if (animData.id == 0) {
		animData.isActive = true;
	}

	//for (AnimData& anim : animData_) {
	//	// 同じ名前が入力されたらassert
	//	if (anim.name == animData.name) {
	//		WinApp::Log("\nduplicate animation name!!\n\n");
	//		assert(false);
	//	}
	//	else {
	//		animData_.push_back(animData);
	//	}
	//}

	//// リストが空なら登録
	//if (animData_.empty()) {
		animData_.push_back(animData);
	//}

	// アニメーションのIDをインクリメント
	animId_++;
}