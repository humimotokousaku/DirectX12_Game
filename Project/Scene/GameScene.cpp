#include "GameScene.h"
#include "GameManager.h"

void GameScene::Initialize() {
	sceneNum = GAME_SCENE;

	// ゲームシステムを生成
	gameSystem_ = std::make_unique<GameSystem>();
	gameSystem_->Initialize();
}

void GameScene::Update() {
	gameSystem_->Update(sceneNum);
}

void GameScene::Draw() {
	gameSystem_->Draw();
}

void GameScene::Finalize() {

}
