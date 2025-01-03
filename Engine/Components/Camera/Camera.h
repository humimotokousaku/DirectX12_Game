#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
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

	/// <summary>
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();

#pragma region Getter
	// スケールの取得
	Vector3 GetScale()const { return worldTransform_.scale; }
	// ローカル座標の取得
	Vector3 GetTranslate()const { return worldTransform_.translate; }
	// ワールド座標の取得
	Vector3 GetWorldPosition() const{
		Vector3 worldPosition = {
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2]
		};
		return worldPosition;
	}
	// 角度の取得
	Vector3 GetRotate() const { return worldTransform_.rotate; }
	// viewProjectionの取得
	ViewProjection GetViewProjection() { return viewProjection_; }
#pragma endregion

#pragma region Setter
	// スケールをセット
	void SetScale(const Vector3& scale) { worldTransform_.scale = scale; }
	// 座標をセット
	void SetTranslate(Vector3 pos) { worldTransform_.translate = pos; }
	// 角度をセット
	void SetRotate(const Vector3& rotate) { worldTransform_.rotate = rotate; }
	// 垂直方向の視野角をセット
	void SetFovY(const float& fovY) { viewProjection_.fovAngleY = fovY; }
	// ビューポートのアスペクト比のセット
	void SetAspectRatio(const float& aspectRatio) { viewProjection_.aspectRatio = aspectRatio; }
	// 深度限界(手前側)のセット
	void SetNearClip(const float& nearZ) { viewProjection_.nearZ = nearZ; }
	// 深度限界(奥側)のセット
	void SetFarClip(const float& farZ) { viewProjection_.farZ = farZ; }
	// viewMatrixのセット
	void SetViewMatrix(Matrix4x4 matrix) { viewProjection_.matView = matrix; }
#pragma endregion

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
	/// ビュー行列を更新する
	/// </summary>
	void UpdateViewMatrix();
	/// <summary>
	/// 射影行列を更新する
	/// </summary>
	void UpdateProjectionMatrix();

public:// 定数
	const float kDefaultFov = 45.0f;

public:// パブリックなメンバ変数
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
private:// プライベートなメンバ変数

};