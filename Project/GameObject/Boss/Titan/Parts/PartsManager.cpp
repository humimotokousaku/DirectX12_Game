#include "PartsManager.h"

PartsManager::~PartsManager() {
	models_.clear();
	for (IPart* part : parts_) {
		delete part;
	}
}

void PartsManager::Initialize(WorldTransform* object3d, Camera* camera, EnemyManager* enemyManager, int id) {
	modelManager_ = ModelManager::GetInstance();
	// アドレス取得
	camera_ = camera;
	enemyManager_ = enemyManager;
	// パーツの親
	rootObject_ = object3d;

	// モデルの読み込み
	modelManager_->LoadModel("", "cube.obj");
	/// モデルの登録
	// 右手[0]
	models_.push_back(modelManager_->SetModel("", "cube.obj"));
	// 左手[1]
	models_.push_back(modelManager_->SetModel("", "cube.obj"));
	// 弱点[2]
	models_.push_back(modelManager_->SetModel("", "cube.obj"));

	/// 体のパーツを作成
	// 右手
	CreateRightHand(id);
	// 左手
	CreateLeftHand(id);
}

void PartsManager::Update() {
	// 死んだパーツを削除
	parts_.remove_if([](IPart* part) {
		if (part->IsDead()) {
			delete part;
			return true;
		}
		return false;
		});
	for (IPart* part : parts_) {
		part->Update();
	}

	ImGui::Begin("Parts");
	ImGui::DragFloat3("BodyPos", &rootObject_->translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("BodyRot", &rootObject_->rotate.x, 0.01f, -6.28f, 6.28f);
	ImGui::End();
}

void PartsManager::Draw() {
	for (IPart* part : parts_) {
		part->Draw();
	}
}

void PartsManager::CreateRightHand(int id) {
	RightHand* rightHand = new RightHand();
	rightHand->Initialize(rootObject_, camera_, models_[0], id);
	rightHand->SetPlayer(player_);
	rightHand->SetEnemyManager(enemyManager_);
	parts_.push_back(rightHand);
}

void PartsManager::CreateLeftHand(int id) {
	LeftHand* leftHand = new LeftHand();
	leftHand->Initialize(rootObject_, camera_, models_[1], id);
	leftHand->SetPlayer(player_);
	leftHand->SetEnemyManager(enemyManager_);
	parts_.push_back(leftHand);
}

void PartsManager::CreateWeakPoint(int id) {

}
