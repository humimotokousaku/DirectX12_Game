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
	void Initialize(std::vector<Vector3> controlPoints);
	void Initialize(std::vector<Vector3> controlPoints, Camera* camera);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// カメラの移動ルートの表示
	/// </summary>
	void MoveRouteDraw();

	///
	/// Getter
	///

	// カメラを取得
	Camera* GetCamera() { return camera_.get(); }
	// カメラのワールド座標を取得
	const WorldTransform& GetWorldTransform() { return camera_->worldTransform_; }

	///
	/// Setter
	///

	void SetTranslation(Vector3 pos) { camera_->worldTransform_.translate = pos; }

public:// パブリックなメンバ変数
	// 線分の数
	static const size_t segmentCount = 100;

private:
	std::unique_ptr<Camera> camera_;

	// スプライン曲線制御点（通過点）
	std::vector<Vector3> controlPoints_;
	// 線分で描画する用の頂点リスト
	std::vector<Vector3> pointsDrawing_;
	// 移動ルートの線(デバッグ用)
	std::array<std::unique_ptr<Line>, segmentCount> line_;

	Vector3 target_;
	float t_;
	float targetT_;
	bool isMoveCamera_;
};