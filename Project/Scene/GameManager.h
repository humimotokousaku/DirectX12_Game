#pragma once
#include "Framework.h"

class GameManager : public Framework
{	
public:
	///
	/// Default Method
	/// 
	// コンストラクタ
	GameManager();

	// デストラクタ
	~GameManager() override;

	// 初期化
	void Initialize() override;

	// 更新処理
	void Update()override;

	// 描画処理
	void Draw() override;

	// 解放処理
	void Finalize() override;

	///
	/// user method
	///

	// 描画前の処理
	void BeginFrame() override;

	// 描画後の処理
	void EndFrame() override;

private:
	// scene
	std::array<std::unique_ptr<IScene>, 4u> sceneArr_;
	int sceneNum_;
	int preSceneNum_;
};

