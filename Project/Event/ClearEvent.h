#pragma once
#include "FollowCamera.h"
#include "Player.h"

class ClearEvent {
public:
	ClearEvent() = default;
	~ClearEvent() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Player* player, FollowCamera* followCamera);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

#pragma region Setter
	/// <summary>
	/// クリア演出を行う
	/// </summary>
	void Start() {
		if (isActive_ || isEnd_) { return; }

		isActive_ = true;
		// 当たり判定を消す
		player_->SetIsCollion(false);
		player_->SetCameraOffset(Vector3{ 0,0,0 });
		// 角度を0にする
		player_->ResetAngle();
		gameClear_->isActive_ = true;

		// クリア時のUIアニメーション
		gameClearUIAnim_.SetIsStart(true);
		// クリア時にカメラを自機の後ろに移動するアニメーション
		cameraOffset_ = followCamera_->GetTargetOffset();
		gameClearCameraOffsetAnim_.SetIsStart(true);
		// クリア時にカメラを回転させるアニメーション
		gameClearCameraAngleAnim_.SetIsStart(true);

		followObject_.translate = player_->GetWorldPosition();
		followObject_.UpdateMatrix();
		followCamera_->SetParent(&followObject_);

		// ラジアルブラーを消す
		PostEffectManager::GetInstance()->radialBlurData_.isActive = false;
		// ヴィネットを消す
		PostEffectManager::GetInstance()->vignetingData_.isActive = false;
		// コントローラーの振動を消す
		Input::GetInstance()->GamePadVibration(0, 0, 0);
		// 時間の速さを戻す
		GameTimer::GetInstance()->SetTimeScale(1.0f);
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

	// ゲームクリアUI
	std::unique_ptr<Sprite> gameClear_;

	// 追従対象のオブジェクト
	WorldTransform followObject_;

	// クリア時のUIアニメーション
	Animation gameClearUIAnim_;
	// クリア時にカメラを自機の後ろに移動するアニメーション
	Animation gameClearCameraOffsetAnim_;
	// クリア時にカメラを回転させるアニメーション
	Animation gameClearCameraAngleAnim_;

	// 自機とカメラの距離
	Vector3 cameraOffset_;
	Vector3 cameraAngle_;
	Vector3 cameraPos_;

	Vector3 velocity_;

	// 開始演出を行うか
	bool isActive_;
	// 演出を終了したか
	bool isEnd_;
};