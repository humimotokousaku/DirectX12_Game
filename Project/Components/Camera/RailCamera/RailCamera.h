#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Vector3.h"
#include "Sphere.h"
#include "Animation.h"

class RailCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::vector<Vector3> controlPoints);

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

	/// <summary>
	/// 自機が加速しているときのカメラ処理
	/// </summary>
	void BoostUpdate();

#pragma region Getter
	// カメラを取得
	Camera* GetCamera() { return camera_.get(); }
	// カメラのワールド座標を取得
	const WorldTransform& GetWorldTransform() { return camera_->worldTransform_; }
	WorldTransform* GetWorldTransform_P() { return &camera_->worldTransform_; }
	// 方向ベクトルを取得
	Vector3* GetDirectionVelocity() { return &velocity_; }
	// レールの何パーセント進んだかを取得
	float* GetRailPercentage() { return &t_; }
	// Fov
	float* GetFov() { return &camera_->viewProjection_.fovAngleY; }
	// レールカメラが終点についたかを取得
	bool GetIsGameClear() { return isGameClear_; }
#pragma endregion

#pragma region Setter
	/// <summary>
	/// カメラの座標を設定
	/// </summary>
	/// <param name="pos"></param>
	void SetTranslation(Vector3 pos) { camera_->worldTransform_.translate = pos; }

	/// <summary>
	/// 自機の移動ベクトルをアドレスで設定
	/// </summary>
	/// <param name="moveVel">Z方向の移動ベクトルのアドレス</param>
	void SetPlayerMoveVel(float* moveVelZ) { moveVelZ_ = moveVelZ; }
	/// <summary>
	/// 加速モードかを設定
	/// </summary>
	/// <param name="isBoost"></param>
	void SetIsBoost(bool* isBoost) { isBoost_ = isBoost; }
#pragma endregion

public:// パブリックなメンバ変数
	// 線分の数
	static const size_t segmentCount = 500;

	// レールカメラが終点に着くまでにかかる時間
	const float kTimeToEndPoint = 1000;

private:
	std::unique_ptr<Camera> camera_;

	// ブースト時のFovアニメーション
	Animation boostFovAnim_;

	// スプライン曲線制御点（通過点）
	std::vector<Vector3> controlPoints_;
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

	// 速度
	Vector3 velocity_;
	// 自機のZ方向の移動ベクトル(自機の移動ベクトルのアドレスをもらう)
	float* moveVelZ_;

	// 現在のフレーム
	float currentFrame_;

	// カメラの凝視する座標
	Vector3 target_;
	// 凝視座標の進行度
	float targetT_;
	// カメラ本体の進行度
	float t_;

	// カメラが移動中か
	bool isMove_;
	// ゲームクリア
	bool isGameClear_;
	// 加速モード(自機の加速フラグのアドレスをもらう)
	bool* isBoost_;
};