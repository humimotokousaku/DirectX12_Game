#pragma once
#include "Enemy.h"
#include "Player.h"

class AimAssist {
public:
	/// 
	/// Default Method
	/// 

	static AimAssist* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	///
	/// User Method
	/// 

	/// <summary>
	/// ワールド座標からスクリーン座標に変換
	/// </summary>
	/// <param name="worldPos"></param>
	/// <returns></returns>
	Vector2 ConvertWorld2Screen(Vector3 worldPos);

	/// <summary>
	/// ロックオン
	/// </summary>
	/// <param name="reticlePos">レティクルのスクリーン座標</param>
	void LockOn(Vector2 reticlePos);

#pragma region Setter
	// 全敵のアドレスを設定
	void SetEnemyList(std::list<Enemy*> enemys) { enemys_ = enemys; }
	// 自機のアドレスを設定
	void SetPlayer(Player* player) { player_ = player; }
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// ロックオンしてないときの3Dレティクル座標を設定
	/// </summary>
	/// <param name="reticlePos">3Dレティクル座標</param>
	void Set3DReticle(Vector3 reticlePos) { reticle3DPos_ = reticlePos; }
#pragma endregion

#pragma region Getter

#pragma endregion

private:
	Camera* camera_;

	std::list<Enemy*> enemys_;
	Player* player_;

	// ロックオンしてないときの3Dレティクル座標を保存
	Vector3 reticle3DPos_;

	// ロックオン中か
	bool isLockOn_;
};