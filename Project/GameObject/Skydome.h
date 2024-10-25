#pragma once
#include "Object3D.h"

class Skydome {
public:
	Skydome() = default;
	~Skydome() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">見た目のモデル</param>
	/// <param name="camera">カメラのアドレス</param>
	void Initialize(Model* model, Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// 3Dオブジェクト
	Object3D object3d_;
};