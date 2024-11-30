#pragma once
#include "Collider.h"
#include "IFixedTurretState.h"
#include "IEnemy.h"
#include "EnemyBullet.h"
#include <vector>

class FixedTurret : public IEnemy {
public:
	FixedTurret();
	~FixedTurret() override;

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

	// 衝突判定
	void OnCollision(Collider* collider) override;

	///
	/// User Method
	/// 

	/// <summary>
	/// 曲線に沿って移動
	/// </summary>
	void Move();

	/// <summary>
	/// 自機を狙って体を向ける
	/// </summary>
	void Aim();

	/// <summary>
	/// 発射処理
	/// </summary>
	void Fire();

	void ChangeState(IFixedTurretState* pState);

#pragma region Getter
	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }
#pragma endregion

public:// 定数
	// 移動速度
	const float kMoveSpeed = 0.005f;

private:
	// 敵のテクスチャ
	uint32_t enemyTexture_;

	// 弾
	std::list<EnemyBullet*> bullets_;

	// 状態遷移
	IFixedTurretState* state_;
};

