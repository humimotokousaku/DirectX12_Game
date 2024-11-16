#pragma once
#include "Enemy.h"
#include "Player.h"

class AimAssist {
private:
	// ロックオン時のレティクル情報
	struct LockOnReticleData {
		Vector3 targetPos;		// ロックオン対象の座標
		Vector3 reticleOffset;	// レティクルの補間
	};

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

	// オブジェクトが反対方向にあるか判定する関数
	bool IsObjectInOppositeDirection(const Vector3& objectPosition);

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
	void LockOn();

#pragma region Setter
	// 全敵のアドレスを設定
	void SetEnemyList(std::list<IEnemy*> enemys) { enemys_ = enemys; }
	// 自機のアドレスを設定
	void SetPlayer(Player* player) { player_ = player; }
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// カメラの方向ベクトルを設定
	/// </summary>
	/// <param name="dirVel"></param>
	void SetCameraDirectionVelocity(Vector3* dirVel) { cameraDirVel_ = dirVel; }
#pragma endregion

#pragma region Getter
	// ロックオンかを取得
	bool* GetIsLockOn() { return &isLockOn_; }
	// ロックオン時のレティクルの補間量を取得
	Vector3* GetLockOnReticleOffset() { return &lockOnReticleOffset_; }
#pragma endregion

public:// 定数
	// ロックオンを開始する範囲(スクリーン座標基準)
	const float kLockOnRange = 64.0f;
	// ロックオンを解除に必要な範囲(スクリーン座標基準)
	const float kLockOnDisabledDist = 64.0f;

	// ロックオン可能な敵との距離(ワールド座標基準)
	const float kLockOnDistance = 200.0f;

private:
	Camera* camera_;

	std::list<IEnemy*> enemys_;
	Player* player_;

	// カメラの方向ベクトル
	Vector3* cameraDirVel_;

	// ロックオン対象の座標
	std::array<Vector3, 20> targetPos_;	

	// ロックオン時のレティクルの補間量
	Vector3 lockOnReticleOffset_;

	// 現在のロックオン数
	int lockOnCount_;

	// ロックオン中か
	bool isLockOn_;
};