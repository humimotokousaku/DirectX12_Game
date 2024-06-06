#pragma once
#include "IScene.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "Line.h"
#include "Plane.h"
#include "Particles.h"
#include "PostEffect.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Object3D.h"
#include "Animation.h"
#include "GameObject/Player.h"
#include "GameObject/Enemy.h"
#include "Collision/CollisionManager.h"

class GameManager;

class TitleScene : public IScene
{
public:
	/// 
	/// Default Method
	/// 

	~TitleScene()override = default;

	// 初期化
	void Initialize() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	// 解放処理
	void Finalize()override;

	///
	/// User Method
	/// 

private:
	std::array<std::unique_ptr<Object3D>, 2> plane_;
	std::unique_ptr<Object3D> axis_;
	std::array<std::unique_ptr<Object3D>, 2> box_;
	std::array<std::unique_ptr<Object3D>, 2> human_;

	std::unique_ptr<Animation> anim_;
	std::unique_ptr<CollisionManager> collisionManager_;
	std::unique_ptr<Player> player_;
	//std::array<std::unique_ptr<Enemy> , 2u> enemy_;
	// パーティクル
	std::unique_ptr<Particles> particle_;

	// テクスチャ
	uint32_t uvcheckerTexture_;
	uint32_t monsterBallTexture_;
	uint32_t particleTexture_;
	uint32_t blockTexture_;
	uint32_t ddsTexture_;

	int textureNum_;
	Input* input_;
	ViewProjection viewProjection_;
	WorldTransform world_;
	std::unique_ptr<Camera> camera_;

	uint32_t bgm_[4];
};