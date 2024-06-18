#pragma once
#include "Object3D.h"
#include "Collider.h"
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

	/// <summary>
	/// レティクルの配置
	/// </summary>
	void Deploy3DReticle();

	/// <summary>
	/// 2Dレティクルの配置
	/// </summary>
	void Deploy2DReticle(const ViewProjection& viewProjection);

	// 自機の回転処理
	void Rotate();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack();

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	///
	/// Setter
	/// 
	
	// モデルを追加
	void AddModel(Model* model) { models_.push_back(model); }

	// レティクルの座標
	//Vector3 GetWorld3DReticlePosition();

	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// ゲームシーンのアドレスを設定
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

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

private:// プライベートなメンバ変数
	// キーボード入力
	Input* input_ = nullptr;

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

	bool isDead_ = true;

	float gameSpeed_;

	// 3Dレティクルの座標
	std::unique_ptr<Object3D> object3dReticle_;
};