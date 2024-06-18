#pragma once
#include "IPrimitive.h"

class Cube : public IPrimitive {
public:
	/// 
	/// Default Method
	/// 
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Cube();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Cube()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="textureHandle">テクスチャの番号</param>
	void Draw(uint32_t textureHandle)override;

	/// <summary>
	/// カメラの設定
	/// </summary>
	/// <param name="camera">カメラのアドレス</param>
	void SetCamera(Camera* camera) { IPrimitive::SetCamera(camera); }
	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="pos">座標</param>
	void SetPosition(Vector3 pos) { worldTransform_.translate = pos; }
	/// <summary>
	/// スケールの設定
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(Vector3 scale) { worldTransform_.scale = scale; }
	/// <summary>
	/// 回転角を設定
	/// </summary>
	/// <param name="rotate">回転角</param>
	void SetRotate(Vector3 rotate) { worldTransform_.rotate = rotate; }
};

