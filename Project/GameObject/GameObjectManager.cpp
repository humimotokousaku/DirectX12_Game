#include "GameObjectManager.h"

GameObjectManager* GameObjectManager::GetInstance() {
	static GameObjectManager instance;
	return &instance;
}

void GameObjectManager::Initialize() {

}

void GameObjectManager::Update() {
	// 自機の距離によってオブジェクトを描画するかを決める(ただし天球は対象外)
	CheckIsDraw();
}

void GameObjectManager::Finalize() {
	gameObjectList_.clear();
}

void GameObjectManager::CheckIsDraw() {
	for (IObject3D* gameObject : gameObjectList_) {
		Vector3 p2o = gameObject->worldTransform.GetWorldPosition() - player_->GetWorldPosition();
		float radius = radius_;
		// 範囲内なら描画する
		if (p2o.x * p2o.x + p2o.y * p2o.y + p2o.z * p2o.z <= radius * radius) {
			gameObject->SetIsActive(true);
			//gameObject->SetIsCollision(true);
		}
		else {
			// モデルの非表示
			gameObject->SetIsActive(false);
			// 当たり判定をとらない
			//gameObject->SetIsCollision(false);
		}
	}
}
