#pragma once
#include "Collider.h"
#include "BaseFixedTurretState.h"
#include "BaseEnemy.h"
#include "EnemyBullet.h"
#include <vector>

/// <summary>
/// 弾を撃つ敵
/// </summary>
class FixedTurret : public BaseEnemy {
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

	void ChangeState(BaseFixedTurretState* pState);

#pragma region Getter
	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }
#pragma endregion

public:// 定数
	// 体の大きさ
	const Vector3 kBodySize = { 1.5f,1.5f,1.5f };

	// 移動速度
	const float kMoveSpeed = 0.005f;

	// 最大体力
	const float kMaxHp = 50.0f;

	// 自機に与える体のダメージ
	const float kBodyDamage = 30.0f;

	// 倒されたときのスコア
	const int kScoreValue = 50;

private:
	// 敵のテクスチャ
	uint32_t enemyTexture_;

	// 弾
	std::list<EnemyBullet*> bullets_;

	// 状態遷移
	BaseFixedTurretState* state_;
};

