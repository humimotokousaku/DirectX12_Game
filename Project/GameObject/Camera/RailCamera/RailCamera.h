#pragma once
#include "Animation.h"
#include "Camera.h"
#include "Model.h"
#include "Player.h"
#include "Sphere.h"
#include "Vector3.h"
#include "WorldTransform.h"

/// <summary>
/// レールカメラ
/// </summary>
class RailCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::vector<Vector3> controlPoints, Player* player);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// カメラの移動ルートの表示(表示しているとだんだん重くなるので基本的に使わない)
	/// </summary>
	void MoveRouteDraw();

	///
	/// User Method
	/// 
private:// プライベートなメンバ関数
	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();
	
	/// <summary>
	/// 自機が加速しているときのカメラ処理
	/// </summary>
	void BoostUpdate();
	/// <summary>
	/// 自機が回避状態の時のカメラ処理
	/// </summary>
	void EvasionUpdate();
	/// <summary>
	/// 自機がジャスト回避状態の時のカメラ処理
	/// </summary>
	void JustEvasionUpdate();

	// カメラの進行状況をリセット
	void Reset();

	/// <summary>
	/// ImGuiの表示
	/// </summary>
	void ImGuiParameter();

public:// GetterとSetter
#pragma region Getter
	// カメラを取得
	Camera* GetCamera() { return camera_.get(); }
	// カメラのワールド座標を取得
	const WorldTransform& GetWorldTransform() { return camera_->worldTransform_; }
	// 方向ベクトルを取得
	Vector3 GetDirectionVelocity() { return velocity_; }
	// レールの何パーセント進んだかを取得
	float GetRailPercentage() { return t_; }
	// 視野角を取得
	float* GetFov() { return &camera_->viewProjection_.fovAngleY; }
	// レールカメラが終点についたかを取得
	bool GetIsGameClear() { return isGameClear_; }
#pragma endregion

#pragma region Setter
	///カメラの座標を設定
	void SetTranslation(Vector3 pos) { camera_->worldTransform_.translate = pos; }
	
	// 移動中かを設定
	void SetIsMove(bool isMove) { isMove_ = isMove; }
#pragma endregion

public:// パブリックなメンバ変数
	// 線分の数
	static const size_t segmentCount = 500;

	// 加速中のfov増加量の最大値
	float kMaxBoostFovIncrease = 30.0f;
	// 回避中のfov増加量の最大値
	float kMaxEvasionFovIncrease = -3.0f;
	// ジャスト回避時のfov増加量の最大値
	float kMaxJustEvasionFovIncrease = -20.0f;

	// 通常の移動速度
	float kNormalMoveSpeed = 0.6f;
	// 加速時の移動速度
	float kBoostMoveSpeed = 0.8f;

private:
	// カメラ
	std::unique_ptr<Camera> camera_;
	// 自機のアドレス
	Player* player_;

	// 加速時のFovアニメーション
	Animation boostFovAnim_;
	// 回避時のFovアニメーション
	Animation evasionFovAnim_;
	// ジャスト回避時のFovアニメーション
	Animation justEvasionFovAnim_;

	// スプライン曲線制御点（通過点）
	std::vector<Vector3> controlPoints_;

	// レールカメラの進む速さ
	float moveSpeed_;

	// 移動ベクトル
	Vector3 velocity_;

	// 加速中のfovの増加量
	float boostFov_;
	// 回避中のfovの増加量
	float evasionFov_;
	// ジャスト回避中のfovの増加量
	float justEvasionFov_;

	// 注視点の座標
	Vector3 target_;
	// 注視点の進行度
	float targetT_;
	// カメラ本体の進行度
	float t_;

	// 現在のフレーム
	float currentFrame_;

	// カメラが移動中か
	bool isMove_;
	// ゲームクリア
	bool isGameClear_;

#pragma region デバッグ用
	// 線分で描画する用の頂点リスト
	std::vector<Vector3> pointsDrawing_;
	// 移動ルートの線(デバッグ用)
	std::array<std::unique_ptr<Line>, segmentCount> line_;
	// 注視点
	Sphere sphere_;
	// デバッグ用の速度
	Vector3 debugVel_;
#pragma endregion
};