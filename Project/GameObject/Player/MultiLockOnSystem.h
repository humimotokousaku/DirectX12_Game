#pragma once
#include "AimAssist/AimAssist.h"
#include "IEnemy.h"
#include "PlayerBullet.h"
#include "Sprite.h"
#include "PostEffectManager.h"
#include <set>

/// <summary>
/// マルチロックオン機能
/// </summary>
class MultiLockOnSystem {
public:// 構造体
	struct MultiLockOnData {
		std::shared_ptr<Sprite> reticleSprite;  // 2Dレティクル
		Animation reticleAnim;					// ロックオン時のアニメーション
		WorldTransform worldTransform;			// ロックオン対象の3D座標
	};

public:
	MultiLockOnSystem() = default;
	~MultiLockOnSystem() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">カメラのアドレス</param>
	void Initialize(Camera* camera);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	///
	/// User Method
	/// 
private:// プライベートなメンバ関数
	/// <summary>
	/// ロックオンの更新処理
	/// </summary>
	void LockOnUpdate();

	/// </summary>
	/// ワールド座標からスクリーン座標に変換
	/// </summary>
	/// <param name="worldPos"></param>
	/// <returns></returns>
	Vector2 ConvertWorld2Screen(Vector3 worldPos);

	/// </summary>
	/// 画面内にオブジェクトがいるかを検出
	/// </summary>
	/// <param name="worldPos"></param>
	/// <returns></returns>
	bool IsObjectInScreen(Vector3 worldPos);

	bool IsObjectInOppositeDirection(const Vector3& objectPosition);

	// 全敵のアドレスを設定
	void SetEnemyList(std::list<IEnemy*> enemys) { enemys_ = enemys; }

private:// 定数
	// 同時ロックオン可能数
	static const int kMaxLockOnNum = 20;

private:// エンジン機能
	// ポストエフェクト
	PostEffectManager* postEffectManager_;

private:
	// ロックオン機能
	AimAssist* aimAssist_;
	// カメラのアドレス
	Camera* camera_;
	// 全ての敵
	std::list<IEnemy*> enemys_;



	// マルチロックオンの情報
	std::vector<MultiLockOnData> multiLockOnDatas_;
	// ロックオンされている敵のID
	std::vector<int> IDs_;
};