#pragma once
#include "CollisionManager.h"
#include "MathStructs.h"
#include "Player.h"
#include "GameTime.h"

class EnemyManager;
/// <summary>
/// 敵の基底クラス
/// </summary>
class BaseEnemy {
public:
	BaseEnemy() = default;
	virtual ~BaseEnemy() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Vector3 pos, Vector3 rotate, int id) = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 衝突応答
	/// </summary>
	/// <param name="collider">当たった相手の衝突情報</param>
	virtual void OnCollision(Collider* collider) = 0;

	/// <summary>
	/// 被弾時のパーティクル生成
	/// </summary>
	void CreateHitParticle();

	// 被ダメージのパーティクルの更新処理
	void HitParticleUpdate(Particle& particle);

	///
	/// User Method
	/// 

	// 敵モデルを追加
	void AddModel(Model* model) {
		models_.push_back(model);
	}

#pragma region Getter
	// 管理番号の取得
	int GetId() { return id_; }
	// スコアの取得
	int GetScore() { return score_; }
	// 移動速度を取得
	Vector3 GetMoveSpeed() { return moveSpeed_; }
	// 完了ならtrueを返す
	bool IsDead() const { return isDead_; }
	// 活動可能状態かを取得
	bool GetIsActive() { return isActive_; }
	// ロックオンできる状態か
	bool GetIsLockOnAvailable() { return isLockOnAvailable_; }
	// 移動中かを取得
	bool GetIsMove() {
		// 終了している
		if (targetT_ >= 1.0f) { return false; }
		// 移動中
		return true;
	}
	WorldTransform* GetWorldTransform() { return &object3d_->worldTransform; }
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
	// ローカル座標の取得
	Vector3 GetLocalPosition() { return object3d_->worldTransform.translate; }
	// 回転角の取得
	Vector3 GetRotation() {
		Vector3 rotate = object3d_->worldTransform.rotate;
		if (object3d_->worldTransform.parent_) {
			rotate += object3d_->worldTransform.parent_->rotate;
		}
		return rotate;
	}
#pragma endregion

#pragma region Setter
	// 自機のアドレスを設定
	void SetPlayer(Player* player) { player_ = player; }
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// エネミーマネージャのアドレスを設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }

	// 移動速度を設定
	void SetMoveSpeed(const Vector3& speed) { moveSpeed_ = speed; }

	// 死亡フラグを設定
	void SetIsDead(const bool& isDead) { isDead_ = isDead; }

	// 機能停止にするかを設定
	// falseなら機能停止
	void SetIsActive(const bool& isActive) {
		isActive_ = isActive;
		// 描画しない
		object3d_->SetIsActive(isActive);
	}

	/// <summary>
	/// 親子関係の設定
	/// </summary>
	/// <param name="parent">親のワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent) {
		object3d_->worldTransform.parent_ = parent;
		object3d_->worldTransform.UpdateMatrix();
	}

	// 移動ルートの制御点の座標の設定
	void SetTravelRouteControlPoints(const std::vector<Vector3>& controlPoints) { controlPoints_ = controlPoints; }

	/// <summary>
	/// ローカル座標を設定
	/// </summary>
	/// <param name="pos">ローカル座標</param>
	void SetLocalPosition(const Vector3& pos) { object3d_->worldTransform.translate = pos; }

	/// <summary>
	/// 被弾パーティクルに使用するテクスチャの設定
	/// </summary>
	/// <param name="textureNum">パーティクルに使用するTexture</param>
	void SetHitParticleTexture(const uint32_t& textureNum) { hitParticleTextures_.particle = textureNum; }
	/// <summary>
	/// 被弾パーティクルのDissolve用のテクスチャの設定
	/// </summary>
	/// <param name="textureNum">Dissolveに使用するTexture</param>
	void SetHitParticleDissolveTexture(const uint32_t& textureNum) { hitParticleTextures_.dissolve = textureNum; }
	/// <summary>
	/// 被弾パーティクルに使用するテクスチャ群の設定
	/// </summary>
	/// <param name="textureNum">パーティクルに使用するTexture</param>
	void SetHitParticleTextures(const ParticleTextures& textureNums) { hitParticleTextures_ = textureNums; }
#pragma endregion

protected:
	// 全てのモデル(見た目のデータ)
	std::vector<Model*> models_;
	// 敵の3DObject
	std::unique_ptr<Object3D> object3d_;

	Player* player_;
	// カメラのアドレス
	Camera* camera_;
	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;
	// エネミーマネージャ
	EnemyManager* enemyManager_ = nullptr;

	// 被弾パーティクルに使用するテクスチャ群
	ParticleTextures hitParticleTextures_;

	// 移動用スプライン曲線制御点（通過点）
	std::vector<Vector3> controlPoints_;
	// 線分で描画する用の頂点リスト
	std::vector<Vector3> pointsDrawing_;
	// 移動ルートの線(デバッグ用)
	std::array<std::unique_ptr<Line>, 10> line_;

	// 敵のタイプ
	std::string type_;

	// 移動速度
	Vector3 moveSpeed_;

	// 注視点の座標
	Vector3 target_;
	// 注視点の進行度
	float targetT_;
	// 移動ルートの進行度
	float t_;

	// 体力
	float hp_ = 100;

	// 管理番号
	int id_;

	// スコア
	int score_;

	// カメラの後ろ側にいるなら描画と機能を停止
	// falseなら機能停止
	bool isActive_ = true;
	// ロックオンのできる敵か
	bool isLockOnAvailable_ = true;
	// 死亡フラグ
	bool isDead_ = false;
};