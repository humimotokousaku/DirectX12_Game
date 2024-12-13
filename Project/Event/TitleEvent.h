#pragma once
#include "Animation.h"
#include "Player.h"
#include "FollowCamera.h"
#include "Skydome/Skydome.h"
#include "LevelManager.h"

/// <summary>
/// タイトル演出
/// </summary>
class TitleEvent {
public:
	TitleEvent() = default;
	~TitleEvent() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// タイトルシーンからゲームシーンになるまでの演出
	/// </summary>
	void TitleSceneToGameSceneEffect();

	/// <summary>
	/// カメラの更新処理
	/// </summary>
	void CameraUpdate();

#pragma region Setter
	/// <summary>
	/// ゲーム開始演出を行うかを設定
	/// </summary>
	void SetIsActive(const bool& isActive) { isActive_ = isActive; }
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

private:// 定数
	// 開始時のカメラオフセット
	const Vector3 kStartCameraOffset = { 0.0f,0.0f,-10.0f };
	// 開始時のカメラの角度
	const Vector3 kStartCameraRotate = { (float)std::numbers::pi / 6.0f, (float)std::numbers::pi, 0.0f };

private:// エンジン機能
	TextureManager* textureManager_;
	ModelManager* modelManager_;
	LevelManager* levelManager_;
	PostEffectManager* postEffectManager_;
	Input* input_;

private:
	// 自機
	std::unique_ptr<Player> player_;
	// 追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	// カメラ
	std::unique_ptr<Camera> camera_;
	// 天球
	std::unique_ptr<Skydome> skydome_;

	// 追従対象のオブジェクト
	WorldTransform followObject_;

	// タイトルシーンからゲームシーンになるまでのカメラアニメーション
	Animation gameStartAngleAnim_;

	Vector3 velocity_;

	// 開始演出を行うか
	bool isActive_;
	// 演出を終了したか
	bool isEnd_;
};