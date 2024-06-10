#pragma once
#include"Object3D.h"
#include "Collision/Collider.h"
#include "Camera.h"
#include "Object3D.h"
#include "Input.h"

class Player : public Collider
{
public:
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(Camera* camera);

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw(uint32_t textureHandle);

	/// Setter
	// モデルを追加
	void AddModel(Object3D* model) { models_.push_back(model); }

	///
	/// 純粋仮想関数
	/// 

	// ワールド座標
	Vector3 GetWorldPosition() override;
	// 角度
	Vector3 GetRotation() override;
	// 衝突判定
	void OnCollision(Collider* collider)override;

public:
	// 自機
	std::unique_ptr<Object3D> object3d_;
	std::vector<Object3D*> models_;
	float gameSpeed_;
};