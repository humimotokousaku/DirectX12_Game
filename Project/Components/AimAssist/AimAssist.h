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
	/// カメラの後ろ側に敵がいるか
	/// </summary>
	/// <param name="cameraPosition">カメラの座標</param>
	/// <param name="cameraDirection">カメラの方向ベクトル</param>
	/// <param name="enemyPosition">敵の座標</param>
	/// <returns>カメラの後ろ側ならtrue</returns>
	bool IsEnemyBehindCamera(const Vector3& cameraPosition, const Vector3& cameraDirection, const Vector3& enemyPosition);

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

	Vector3 reticlePos_;
public:// 定数
	// ロックオンを開始する範囲(スクリーン座標基準)
	const float kLockOnRange = 64.0f;
	// ロックオンを解除のに必要な距離(スクリーン座標基準)
	const float kLockOnDisabledDist = 64.0f;
private:
	Camera* camera_;

	std::list<Enemy*> enemys_;
	Player* player_;

	// カメラの方向ベクトル
	Vector3* cameraDirVel_;

	// ロックオン中か
	bool isLockOn_;

	// ロックオン時のレティクルの補間量
	Vector3 lockOnReticleOffset_;

	// ロックオンした敵の管理番号
	int enemyId_;
};