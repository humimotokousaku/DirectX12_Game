#include "GameScene.h"
#include "GameManager.h"

void GameScene::Initialize() {
	sceneNum = GAME_SCENE;

	// ゲームオブジェクトマネージャー
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Initialize();

	// ゲームシステムを生成
	gameSystem_ = std::make_unique<GameSystem>();
	gameSystem_->Initialize();
}

void GameScene::Update() {
	gameSystem_->Update(sceneNum);

	// 自機との距離に応じてゲームオブジェクトを消す
	gameObjectManager_->Update();
}

void GameScene::Draw() {
	gameSystem_->Draw();

}

void GameScene::Finalize() {

}
