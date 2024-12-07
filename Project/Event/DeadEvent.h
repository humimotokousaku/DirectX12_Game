#pragma once
#include "Player.h"
#include "FollowCamera.h"
#include "RailCamera.h"
#include "Sprite.h"

class DeadEvent {
public:
	DeadEvent() = default;
	~DeadEvent() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Player* player, FollowCamera* followCamera, RailCamera* railCamera);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

#pragma region Setter
	/// <summary>
	/// クリア演出を行う
	/// </summary>
	void Start() {
		if (isActive_ || isEnd_) { return; }

		isActive_ = true;
		player_->SetIsCollion(false);
		gameOver_->isActive_ = true;
		// ゲームオーバーUIのアニメーション開始
		gameOverUIAnim_.SetIsStart(true);

		// ラジアルブラーを消す
		PostEffectManager::GetInstance()->radialBlurData_.isActive = false;
		// ヴィネットを消す
		PostEffectManager::GetInstance()->vignetingData_.isActive = false;
		// コントローラーの振動を消す
		Input::GetInstance()->GamePadVibration(0, 0, 0);
	}
#pragma endregion

#pragma region Getter
	/// <summary>
	/// ゲーム開始演出を行っているかを取得
	/// </summary>
	/// <returns></returns>
	const bool& GetIsActive() { return isActive_; }
	/// <summary>
	/// 全ての開始演出が終了したかを取得
	/// </summary>
	/// <returns></returns>
	const bool& GetIsEnd() { return isEnd_; }
#pragma endregion

private:// エンジン機能
	PostEffectManager* postEffectManager_;

private:
	// 自機のアドレス
	Player* player_;
	// 追従カメラのアドレス
	FollowCamera* followCamera_;
	// レールカメラのアドレス
	RailCamera* railCamera_;

	// ゲームオーバーUI
	std::unique_ptr<Sprite> gameOver_;

	// ゲームオーバーUIのアニメーション
	Animation gameOverUIAnim_;

	// 開始演出を行うか
	bool isActive_;
	// 演出を終了したか
	bool isEnd_;
};

