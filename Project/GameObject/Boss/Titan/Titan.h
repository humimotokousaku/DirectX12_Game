#pragma once
#include "IEnemy.h"
#include "ITitanState.h"
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

private:
	PartsManager* partsManager_;

	// 状態遷移
	ITitanState* state_;
};