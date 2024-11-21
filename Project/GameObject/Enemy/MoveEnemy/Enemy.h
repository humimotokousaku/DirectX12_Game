#pragma once
#include "Collider.h"
#include "IEnemyState.h"
#include "IEnemy.h"
#include "EnemyBullet.h"
#include <vector>

class Enemy : public IEnemy
{
public:
	Enemy();
	~Enemy() override;

	///
	/// 純粋仮想関数
	/// 

	/// <summary>
	/// 初期化(設定すべき項目を全て終わらせてから行う)
	/// <summary>
	void Initialize(Vector3 pos, Vector3 rotate,int id) override;
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
	/// stateの変更
	/// </summary>
	/// <param name="pState">state</param>
	void ChangeState(IEnemyState* pState);

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="velocity">速度</param>
	void Move(const Vector3 velocity);

	/// <summary>
	/// 発射処理
	/// </summary>
	void Fire();

#pragma region Getter
	// 座標を取得
	Vector3 GetEnemyPos() { return object3d_->worldTransform.translate; }
	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }
#pragma endregion

#pragma region Setter
	// 座標を設定
	void SetPosition(Vector3 pos) { object3d_->worldTransform.translate = pos; }
	// 親子関係の設定
	void SetParent(const WorldTransform* parent) { object3d_->worldTransform.parent_ = parent; }
#pragma endregion

private:
	// 敵のテクスチャ
	uint32_t enemyTexture_;

	// 弾
	std::list<EnemyBullet*> bullets_;

	// 状態遷移
	IEnemyState* state_;
};

