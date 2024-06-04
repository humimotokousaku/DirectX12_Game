#pragma once
#include "ViewProjection.h"
#include "ModelStructs.h"
#include "Input.h"

class Camera
{
public:
	/// 
	/// Default Method
	/// 

	Camera() = default;
	~Camera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// 
	/// User Method
	/// 
	
	/// Getter
	// transformの取得
	Transform GetTransform() { return transform_; }
	// スケールの取得
	Vector3 GetScale()const { return transform_.scale; }
	// 座標の取得
	Vector3 GetTranslate()const { return transform_.translate; }
	// 角度の取得
	Vector3 GetRotate() const { return transform_.rotate; }
	// viewProjectionの取得
	ViewProjection GetViewProjection() { return viewProjection_; }

	/// Setter
	// スケールをセット
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	// 座標をセット
	void SetTranslate(Vector3 pos) { transform_.translate = pos; }
	// 角度をセット
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	// 垂直方向の視野角をセット
	void SetFovY(const float& fovY) { viewProjection_.fovAngleY = fovY; }
	// ビューポートのアスペクト比のセット
	void SetAspectRatio(const float& aspectRatio) { viewProjection_.aspectRatio = aspectRatio; }
	// 深度限界(手前側)のセット
	void SetNearClip(const float& nearZ) { viewProjection_.nearZ = nearZ; }
	// 深度限界(奥側)のセット
	void SetFarClip(const float& farZ) { viewProjection_.farZ = farZ; }

private:
	/// <summary>
	/// 初期化
	/// </summary>
	void CreateViewProjection();
	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();
	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();
	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();
	/// <summary>
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();
	/// <summary>
	/// ビュー行列を更新する
	/// </summary>
	void UpdateViewMatrix();
	/// <summary>
	/// 射影行列を更新する
	/// </summary>
	void UpdateProjectionMatrix();

public:// パブリックなメンバ変数
	Transform transform_;

private:// プライベートなメンバ変数
	ViewProjection viewProjection_;
};