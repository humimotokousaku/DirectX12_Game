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
#include "Cube.h"
#include "IPrimitive.h"
#include "Animation.h"
#include "Player.h"
#include "Enemy.h"
#include "RailCamera.h"
#include "CollisionManager.h"

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
	Input* input_;

	std::array<std::unique_ptr<Object3D>, 2> plane_;
	std::unique_ptr<Object3D> axis_;
	std::array<std::unique_ptr<Object3D>, 2> box_;
	std::array<std::unique_ptr<Object3D>, 2> human_;
	// skybox
	std::unique_ptr<Cube> cube_;
	// UI
	std::array<std::unique_ptr<Sprite>, 2> guideUI_;
	// カメラ
	std::unique_ptr<Camera> camera_;
	// レールカメラ
	std::unique_ptr<RailCamera> railCamera_;

	// テクスチャ
	uint32_t uvcheckerTexture_;
	uint32_t monsterBallTexture_;
	uint32_t particleTexture_;
	uint32_t blockTexture_;
	uint32_t whiteTexture_;
	uint32_t ddsTexture_;
	std::array<uint32_t, 10> numbersTexture_;

	uint32_t bgm_[4];
};