#pragma once
#include "Animation.h"
#include "Sprite.h"
#include "PostEffectManager.h"

class SceneTransition {
public:
	static SceneTransition* GetInstance();
	void Initialize();
	void Update();

	// シーン切り替え開始
	void Start() { isStart_ = true; }

#pragma region Behaviorの関数
	void B_NoneInit();
	void B_NoneUpdate();

	void B_FadeInInit();
	void B_FadeInUpdate();

	void B_FadeOutInit();
	void B_FadeOutUpdate();

	void B_EndInit();
	void B_EndUpdate();
#pragma endregion

#pragma region Getter
	// シーン切り替え可能信号を取得
	bool GetSceneTransitionSignal() { return sceneTransitionSignal_; }
#pragma endregion

#pragma region Setter
	// シーン切り替え可能信号を設定
	void SetSceneTransitionSignal(bool isActive) { sceneTransitionSignal_ = isActive; }
	// ポストエフェクトを適用しないスプライトを再登録
	void SetSpriteList(){ PostEffectManager::GetInstance()->AddSpriteList(&sprite_); }
#pragma endregion

private:
	// 振るまい
	enum class Behavior {
		None,	 // 通常状態
		FadeIn,// ダッシュ攻撃前の硬直
		FadeOut,	 // ダッシュ中
		End
	};
	// 現在の振るまい
	Behavior behavior_ = Behavior::None;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Animation beginAnim_;
	Animation endAnim_;
	Sprite sprite_;

	bool isStart_;
	bool sceneTransitionSignal_;
};