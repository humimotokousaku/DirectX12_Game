#pragma once
#include "Object3D.h"
#include <memory>

/// <summary>
/// 天球
/// </summary>
class Skydome {
public:
	Skydome() = default;
	~Skydome() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">見た目のモデル</param>
	/// <param name="camera">カメラのアドレス</param>
	/// <param name="parent">自機のワールドトランスフォーム</param>
	void Initialize(Model* model, Camera* camera, const WorldTransform* parent = nullptr);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// 3Dオブジェクト
	std::unique_ptr<Object3D> object3d_;
};