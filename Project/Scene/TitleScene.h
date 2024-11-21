#pragma once
#include "IScene.h"
#include "Input.h"
#include "Object3d.h"

class GameManager;

class TitleScene : public IScene
{
public:
	~TitleScene()override = default;

	// 初期化
	void Initialize() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	// 解放処理
	void Finalize()override;

private:
	// ddsテクスチャを張り付けたモデル
	std::unique_ptr<Object3D> object3d_;

	// ddsテクスチャ
	uint32_t ddsTexture_;

	// カメラ
	std::unique_ptr<Camera> camera_;
};