#pragma once
#include "Camera.h"
#include "Player.h"
#include "Shake.h"

class FollowCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Player* player);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 被弾時の更新処理
	/// </summary>
	void HitUpdate();

	// 追従対象からのオフセットを計算する
	Vector3 TargetOffset() const;

#pragma region Setter
	// 親子関係を設定
	void SetParent(const WorldTransform* target) {
		camera_->worldTransform_.parent_ = target;
	}

	void SetPlayerPos(Vector3 pos) { playerPos_ = pos; }

	void SetFov(float* fov) { fov_ = fov; }
#pragma endregion

#pragma region Getter
	/// <summary>
	/// カメラのアドレスを取得
	/// </summary>
	/// <returns></returns>
	Camera* GetCamera() { return camera_.get(); }

	// カメラのワールド座標を取得
	const WorldTransform& GetWorldTransform() { return camera_->worldTransform_; }

	/// <summary>
	/// カメラのワールド座標を取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPosition(){
		Vector3 result = {
			camera_->worldTransform_.matWorld_.m[3][0],
			camera_->worldTransform_.matWorld_.m[3][1],
			camera_->worldTransform_.matWorld_.m[3][2]
		};
		return result;
	}
#pragma endregion

public:
	// カメラ補間の精度
	const float followRate = 0.1f;

private:
	// カメラ
	std::unique_ptr<Camera> camera_;
	// 自機のアドレス
	Player* player_;

	// ダメージ時に揺らす
	Shake* hitShake_;
	// 被弾時のカメラの揺れる範囲
	Animation shakeRangeAnim_;

	// 揺れの補正値
	Vector3 shakeOffset_;

	Vector3 playerPos_;
	Vector3 interTarget_;

	float* fov_;
};