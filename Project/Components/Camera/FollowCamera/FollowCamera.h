#pragma once
#include "Camera.h"

class FollowCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	// 追従対象からのオフセットを計算する
	Vector3 TargetOffset() const;

	// 親子関係を設定
	void SetParent(const WorldTransform* target) {
		camera_->worldTransform_.parent_ = target;
	}

	void SetCameraOffset(Vector3* cameraOffset) { offset_ = cameraOffset; }

	void SetPlayerPos(Vector3 pos) { playerPos_ = pos; }

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

private:
	// カメラ
	std::unique_ptr<Camera> camera_;

	Vector3* offset_;

	Vector3 playerPos_;
};