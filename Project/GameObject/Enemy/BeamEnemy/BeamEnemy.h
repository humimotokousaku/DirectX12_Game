#pragma once
#include "IEnemy.h"
#include "IEnemyState.h"

#include "Animation.h"

class BeamEnemy : public IEnemy {
public:
	BeamEnemy();
	~BeamEnemy() override;

	///
	/// 純粋仮想関数
	/// 

	/// <summary>
	/// 初期化(設定すべき項目を全て終わらせてから行う)
	/// <summary>
	void Initialize(Vector3 pos, Vector3 rotate, int id) override;
	/// <summary>
	/// 更新
	/// <summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// <summary>
	void Draw() override;

	// ワールド座標
	Vector3 GetWorldPosition();
	// 角度
	Vector3 GetRotation();
	// 衝突判定
	void OnCollision(Collider* collider) override;

public:// 定数
	// 初期のビームのサイズ
	const Vector3 kDefaultBeamSize = { 1.0f, 10.0f, 1.0f };

private:
	// ビーム攻撃
	std::unique_ptr<Object3D> beamObject_;

	// ビームアニメーション
	Animation beamAnim_;
};