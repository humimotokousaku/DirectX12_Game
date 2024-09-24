#pragma once
#include "Object3D.h"
#include "Collider.h"
#include "Player.h"

class EnemyManager;
class IPart {
public:
	IPart();
	virtual ~IPart() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(const WorldTransform* parent, Camera* camera, Model* model, int id) = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	///
	/// User Method
	/// 
	
#pragma region Getter
	// 管理番号の取得
	int GetId() { return id_; }
	// スコアの取得
	int GetScore() { return score_; }
	// 完了ならtrueを返す
	bool IsDead() const { return isDead_; }
	// 活動可能状態かを取得
	bool GetIsActive() { return isActive_; }
	// ワールド座標の取得
	Vector3 GetWorldPosition() {
		// ワールド座標を入れる変数
		Vector3 worldPos{};
		// ワールド行列の平行移動成分を取得
		worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
		worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
		worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

		return worldPos;
	}
#pragma endregion

#pragma region Setter
	// 自機のアドレスを設定
	void SetPlayer(Player* player) { player_ = player; }
	// エネミーマネージャのアドレスを設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
	// 死亡フラグを設定
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	// 機能停止にするかを設定
	// falseなら機能停止
	void SetIsActive(bool isActive) { 
		isActive_ = isActive;
		// 描画しない
		object3d_->SetIsActive(isActive);
	}
#pragma endregion

protected:	
	// モデル(見た目のデータ)
	Model* model_;
	// 敵の3DObject
	std::unique_ptr<Object3D> object3d_;

	// 自機のアドレス
	Player* player_;
	// カメラのアドレス
	Camera* camera_;
	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;
	// エネミーマネージャ
	EnemyManager* enemyManager_ = nullptr;

	// 死亡フラグ
	bool isDead_ = false;
	float hp_ = 100;

	// 管理番号
	int id_;

	// スコア
	int score_;

	// 敵のタイプ
	std::string type_;

	// カメラの後ろ側にいるなら描画と機能を停止
	// falseなら機能停止
	bool isActive_ = true;
};