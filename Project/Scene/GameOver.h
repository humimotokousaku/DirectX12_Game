#pragma once
#include "IScene.h"

class GameManager;
class GameOver : public IScene
{
public:
	~GameOver()override = default;

	// 初期化
	void Initialize()override;

	// 更新処理
	void Update()override;

	// 描画
	void Draw()override;

	void Finalize()override;
private:

};

