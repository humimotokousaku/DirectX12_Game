#pragma once
#include "Object3D.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "Camera.h"
#include "Sprite.h"
#include "Object3D.h"
#include "Input.h"
#include "PlayerBullet.h"

class GameScene;
class Player : public Collider {
public:// パブリックなメンバ関数
	/// 
	/// Default Method
	/// 

	Player();
	~Player();

	/// <summary>
	/// 初期化(設定すべき項目を全て終わらせてから行う)
	/// <summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw();

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	///
	/// User Method
	/// 

public:
	///
	/// Getter
	/// 

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }
	// レティクルの座標
	Vector3 GetWorld3DReticlePosition();

	///
	/// Setter
	/// 
	
	// モデルを追加
	void AddModel(Model* model) { models_.push_back(model); }

	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// ゲームシーンのアドレスを設定
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	// 衝突マネージャのアドレスを設定
	void SetCollisionManager(CollisionManager* collisionManager) { collisionManager_ = collisionManager; }

	/// <summary>
	/// 親となるワールドトランスフォームを設定
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

	///
	/// 純粋仮想関数
	/// 

	// ワールド座標
	Vector3 GetWorldPosition() override;
	// 角度
	Vector3 GetRotation() override;
	// 衝突判定
	void OnCollision(Collider* collider)override;

private:// プライベートなメンバ関数
	/// <summary>
	/// レティクルの配置
	/// </summary>
	void Deploy3DReticle();

	/// <summary>
	/// 2Dレティクルの配置
	/// </summary>
	void Deploy2DReticle();

	/// <summary>
	/// 自機の回転処理
	/// </summary>
	void Rotate();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack();

	/// <summary>
	/// 対象の座標の補間を算出
	/// </summary>
	/// <returns></returns>
	Vector3 TargetPosOffset();

private:// プライベートなメンバ変数
	// キーボード入力
	Input* input_ = nullptr;
	// 移動に使うキー
	std::vector<int> moveKeys_;
	XINPUT_STATE joyState_;

	// 自機
	std::unique_ptr<Object3D> object3d_;
	std::vector<Model*> models_;
	// 2Dレティクル用のスプライト
	Sprite* sprite2DReticle_ = nullptr;

	// テクスチャハンドル
	uint32_t playerTexture_ = 0u;
	// レティクルハンドル
	uint32_t reticleTexture_ = 0u;

	// 弾
	std::list<PlayerBullet*> bullets_;

	// カメラのアドレス
	Camera* camera_;
	// ゲームシーンのアドレス
	GameScene* gameScene_;
	// 衝突マネージャのアドレス
	CollisionManager* collisionManager_;

	bool isDead_ = true;

	float gameSpeed_;

	// 3Dレティクルの座標
	std::unique_ptr<Object3D> object3dReticle_;

	// 移動速度
	Vector3 velocity_;
};