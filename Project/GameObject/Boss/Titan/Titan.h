#pragma once
#include "IEnemy.h"
#include "ITitanState.h"
#include "AttackAlert.h"
#include "PartsManager.h"
#include <memory>

class Titan : public IEnemy{
public:
	Titan() = default;
	~Titan() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Vector3 pos, Vector3 rotate, int id) override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	// 衝突判定
	void OnCollision(Collider* collider) override;

	// 状態切り替え
	void ChangeState(ITitanState* pState);

	///
	/// User Method
	///
	
#pragma region Getter
	// パーツマネージャーを取得
	PartsManager* GetPartsManager() { return partsManager_; }
#pragma endregion

#pragma region Setter
	// 追従する座標を設定
	void SetParent(const WorldTransform* parent) { followWorldTransform_.parent_ = parent; }
#pragma endregion

private:// エンジン機能
	// ポストエフェクト
	PartsManager* partsManager_;

private:
	// 攻撃の警告
	AttackAlert* attackAlert_;

	// 追従する座標
	WorldTransform followWorldTransform_;

	// 状態遷移
	ITitanState* state_;
};