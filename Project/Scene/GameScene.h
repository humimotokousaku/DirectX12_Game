#pragma once
#include "IScene.h"

class GameManager;
class GameScene : public IScene
{
public:
	~GameScene()override = default;
	GameScene();

	// 初期化
	void Initialize()override;

	// 更新処理
	void Update()override;

	// 描画
	void Draw()override;

	void Finalize()override;
private:

};

