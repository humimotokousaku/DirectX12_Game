#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Vector3.h"

class RailCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Vector3 target);

	///
	/// Getter
	///

	// カメラを取得
	Camera* GetCamera() { return camera_.get(); }
	// カメラのワールド座標を取得
	const WorldTransform& GetWorldTransform() { return camera_->worldTransform_; }

	//ViewProjection& GetViewProjection() { return viewProjection_; }
	//const WorldTransform& GetWorldTransform() { return worldTransform_; }

	///
	/// Setter
	///

	//void SetViewTranslation(Vector3 pos) { viewProjection_.translate = pos; }
	void SetTranslation(Vector3 pos) { camera_->worldTransform_.translate = pos; }

private:
	// ワールド変換データ
	//WorldTransform worldTransform_;
	// ビュープロジェクション
	//ViewProjection viewProjection_;

	std::unique_ptr<Camera> camera_;
};