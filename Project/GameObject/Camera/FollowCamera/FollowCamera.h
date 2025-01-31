#pragma once
#include "Camera.h"
#include "Player.h"
#include "Shake.h"
#include "MultiLockOnSystem.h"

/// <summary>
/// 追従カメラ
/// </summary>
class FollowCamera {
public:
	FollowCamera();
	~FollowCamera() = default;

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

	/// <summary>
	/// 弾発射時の処理
	/// </summary>
	void ShotUpdate();

	/// <summary>
	/// 追従対象からのオフセットを計算する
	/// </summary>
	/// <returns></returns>
	Vector3 TargetOffset() const;

	/// <summary>
	/// 座標を固定する
	/// </summary>
	void PositionLock(const Vector3& lockTarget);

#pragma region Setter
	/// <summary>
	/// マルチロックオンクラスのアドレスを設定
	/// </summary>
	/// <param name="multiLockOnSystem">マルチロックオンクラスのアドレス</param>
	void SetMultiLockOnSystem(MultiLockOnSystem* multiLockOnSystem) { multiLockOnSystem_ = multiLockOnSystem; }
	/// <summary>
	/// 親子関係を設定
	/// </summary>
	/// <param name="target">親</param>
	void SetParent(const WorldTransform* target) {	camera_->worldTransform_.parent_ = target; }

	/// <summary>
	/// 自機の座標を設定
	/// </summary>
	/// <param name="pos">座標</param>
	void SetPlayerPos(const Vector3& pos) { playerPos_ = pos; }

	/// <summary>
	/// 追従対象との距離を設定
	/// </summary>
	/// <param name="offset">距離</param>
	void SetTargetOffset(const Vector3& offset) { targetOffset_ = offset; }

	/// <summary>
	/// 視野角を設定
	/// </summary>
	/// <param name="fov">視野角</param>
	void SetFov(float* fov) { fov_ = fov; }

	/// <summary>
	/// 追従カメラの座標を固定するかを設定
	/// </summary>
	/// <param name="isLock">座標を固定するか</param>
	void SetIsLockPos(const bool& isLock) { isLockPos_ = isLock; }
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
	Vector3 GetWorldPosition() {
		Vector3 result = {
			camera_->worldTransform_.matWorld_.m[3][0],
			camera_->worldTransform_.matWorld_.m[3][1],
			camera_->worldTransform_.matWorld_.m[3][2]
		};
		return result;
	}
	/// <summary>
	/// 追従対象との距離
	/// </summary>
	/// <returns></returns>
	const Vector3& GetTargetOffset() { return targetOffset_; }
#pragma endregion

public:
	// カメラ補間の精度
	const float followRate = 0.1f;

	// 初期オフセット
	const Vector3 kDefaultOffset = { 0,-5,-20 };

private:
	// カメラ
	std::unique_ptr<Camera> camera_;
	// 自機のアドレス
	Player* player_;
	// マルチロックオンクラスのアドレス
	MultiLockOnSystem* multiLockOnSystem_;

	// ダメージ時に揺らす
	Shake* hitShake_;
	// 射撃時に揺らす
	Shake* shotShake_;

	// 被弾時のカメラの揺れる範囲
	Animation shakeRangeAnim_;
	// 射撃時のカメラの揺れる範囲
	//Animation shakeRangeAnim_;

	// 揺れの補正値
	Vector3 shakeOffset_;
	// 射撃時の揺れ補正値
	Vector3 shotShakeOffset_;

	Vector3 playerPos_;
	Vector3 interTarget_;

	Vector3 targetOffset_;

	float* fov_;

	// 座標を動かないようにするか
	bool isLockPos_;
};