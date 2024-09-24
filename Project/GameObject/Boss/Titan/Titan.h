#pragma once
#include "IEnemy.h"
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

private:
	std::unique_ptr<PartsManager> partsManager_;
};