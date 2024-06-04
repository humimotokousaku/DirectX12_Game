#pragma once
#include "Collision/Collider.h"
#include "Camera.h"
#include "Object3D.h"

class Enemy : public Collider
{
public:
	~Enemy();

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

	/// 
	/// Setter
	/// 

	// 座標を指定
	//void SetWorldPosition(Vector3 pos) { model_->worldTransform.transform.translate = pos; }

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
	//std::unique_ptr<Object3D> model_;
};

