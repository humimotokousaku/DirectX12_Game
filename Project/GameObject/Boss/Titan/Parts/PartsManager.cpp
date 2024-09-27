#include "PartsManager.h"

PartsManager::~PartsManager() {
	models_.clear();
	for (auto part = parts_.begin(); part != parts_.end(); ++part) {
		delete part->second;
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
	// 弱点
	CreateWeakPoint(id);

	// 状態変更可能
	isStateChangeable_ = true;
}

void PartsManager::Update() {
	// 死んだパーツを削除
	std::erase_if(parts_, [](const auto& pair) {
		IPart* part = pair.second;
		if (part->IsDead()) {
			delete part;
			return true;
		}
		return false;
		});
	for (auto part = parts_.begin(); part != parts_.end(); ++part) {
		part->second->Update();
	}

	// 振る舞いの更新
	BehaviorUpdate();

#ifdef _DEBUG
	ImGui::Begin("Parts");
	ImGui::DragFloat3("BodyPos", &rootObject_->translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("BodyRot", &rootObject_->rotate.x, 0.01f, -6.28f, 6.28f);
	ImGui::End();
#endif // DEBUG
}

void PartsManager::Draw() {
	for (auto part = parts_.begin(); part != parts_.end(); ++part) {
		part->second->Draw();
	}
}

void PartsManager::CreateRightHand(int id) {
	RightHand* rightHand = new RightHand();
	rightHand->Initialize(rootObject_, camera_, models_[0], id);
	rightHand->SetPlayer(player_);
	rightHand->SetEnemyManager(enemyManager_);
	parts_["RightHand"] = rightHand;
}

void PartsManager::CreateLeftHand(int id) {
	LeftHand* leftHand = new LeftHand();
	leftHand->Initialize(rootObject_, camera_, models_[1], id);
	leftHand->SetPlayer(player_);
	leftHand->SetEnemyManager(enemyManager_);
	parts_["LeftHand"] = leftHand;
}

void PartsManager::CreateWeakPoint(int id) {
	WeakPoint* weakPoint = new WeakPoint();
	weakPoint->Initialize(rootObject_, camera_, models_[2], id);
	weakPoint->SetPlayer(player_);
	weakPoint->SetEnemyManager(enemyManager_);
	weakPoint->SetIsActive(false);
	parts_["WeakPoint"] = weakPoint;
}

void PartsManager::BehaviorUpdate() {
	// 初期化
	if (behaviorRequest_) {
		//  振るまいを変更
		behavior_ = behaviorRequest_.value();
		switch (behavior_) {
		case TitanBehavior::Wait:
		default:
			B_WaitInit();
			break;
		case TitanBehavior::HandAttack:
			B_HandAttackInit();
			break;
		case TitanBehavior::BeamAttack:
			B_BeamAttackInit();
			break;
		case TitanBehavior::RevealWeakPoint:
			B_RevealWeakPointInit();
			break;
		}
		// 振るまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
	// 更新処理
	switch (behavior_) {
	case TitanBehavior::Wait:
	default:
		B_WaitUpdate();
		break;
	case TitanBehavior::HandAttack:
		B_HandAttackUpdate();
		break;
	case TitanBehavior::BeamAttack:
		B_BeamAttackUpdate();
		break;
	case TitanBehavior::RevealWeakPoint:
		B_RevealWeakPointUpdate();
		break;	
	}
}

void PartsManager::B_WaitInit() {
	isStateChangeable_ = true;
}
void PartsManager::B_WaitUpdate() {

}

void PartsManager::B_HandAttackInit() {
	//isStateChangeable_ = false;
}
void PartsManager::B_HandAttackUpdate() {
	for (const auto& pair : parts_) {
		IPart* part = pair.second;
		// dynamic_castを使ってLeftHandかどうかを確認
		LeftHand* leftHandPart = dynamic_cast<LeftHand*>(part);
		RightHand* rightHandPart = dynamic_cast<RightHand*>(part);
		if (leftHandPart) {
			leftHandPart->HandAttack();
		}
		if (rightHandPart) {
			rightHandPart->HandAttack();
		}
	}
}

void PartsManager::B_BeamAttackInit()
{
}
void PartsManager::B_BeamAttackUpdate()
{
}

void PartsManager::B_RevealWeakPointInit()
{
}
void PartsManager::B_RevealWeakPointUpdate()
{
}
