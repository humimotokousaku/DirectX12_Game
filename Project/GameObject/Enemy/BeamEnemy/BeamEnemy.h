#pragma once
#include "IEnemy.h"
#include "IBeamEnemyState.h"

#include "Animation.h"

/// <summary>
/// ビームを撃つ敵
/// </summary>
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

	/// <summary>
	/// stateの変更
	/// </summary>
	/// <param name="pState">state</param>
	void ChangeState(IBeamEnemyState* pState);

	// 衝突判定
	void OnCollision(Collider* collider) override;

	///
	/// User Method
	/// 

#pragma region Getter

#pragma endregion

#pragma region Setter
	// 座標を設定
	void SetPosiotion(Vector3 pos) { object3d_->worldTransform.translate = pos; }
	// 回転角の設定
	void SetRotate(Vector3 rotate) { object3d_->worldTransform.rotate = rotate; }
#pragma endregion

public:// 定数
	// 初期のビームのサイズ
	const Vector3 kDefaultBeamSize = { 2.0f, 50.0f, 2.0f };

	// 移動速度
	const float kMoveSpeed = 0.005f;

	// 最大体力
	const float kMaxHp = 100.0f;

	// 自機に与えるビームのダメージ
	const float kBeamDamage = 30.0f;
	// 自機に与える体のダメージ
	const float kBodyDamage = 30.0f;

private:
	// ビーム攻撃
	std::unique_ptr<Object3D> beamObject_;

	// ビームアニメーション
	Animation beamAnim_;

	// 行動の状態
	IBeamEnemyState* state_;
};