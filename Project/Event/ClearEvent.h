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
	/// ゲーム開始演出を行うかを設定
	/// </summary>
	void SetIsActive(const bool& isActive) { 
		isActive_ = isActive; 
		// クリア時のUIアニメーション
		gameClearUIAnim_.SetIsStart(isActive_);
		// クリア時にカメラを自機の後ろに移動するアニメーション
		gameClearCameraOffsetAnim_.SetIsStart(isActive_);
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

	// 自機とカメラの距離
	Vector3 cameraOffset_;
	Vector3 velocity_;

	// 開始演出を行うか
	bool isActive_;
	// 演出を終了したか
	bool isEnd_;
};