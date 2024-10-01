#include "Titan.h"
#include "TitanWaitState.h"

Titan::~Titan() {
	models_.clear();
	delete partsManager_;
}

void Titan::Initialize(Vector3 pos, Vector3 rotate, int id) {
	// 衝突マネージャーのインスタンスを取得
	collisionManager_ = CollisionManager::GetInstance();
	// 強い攻撃を警告するクラスのインスタンス取得
	attackAlert_ = AttackAlert::GetInstance();

	// 体の生成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_);
	object3d_->SetModel(models_[0]);
	// 体の位置と向きを設定
	object3d_->worldTransform.translate = pos;
	object3d_->worldTransform.rotate = rotate;
	object3d_->worldTransform.UpdateMatrix();
	followWorldTransform_.Initialize();
	object3d_->worldTransform.parent_ = &followWorldTransform_;
	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	object3d_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	object3d_->collider->SetOnCollision(std::bind(&Titan::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(true);

	// パーツの生成
	partsManager_ = new PartsManager();
	partsManager_->SetPlayer(player_);
	partsManager_->SetEnemyManager(enemyManager_);
	partsManager_->Initialize(&object3d_->worldTransform, camera_, enemyManager_, id);


	// 状態遷移
	state_ = new TitanWaitState(this, player_);
	state_->Initialize();

	// HP
	hp_ = 1000;

	// 管理番号
	id_ = id;

	// スコア
	score_ = 4000;

	// ロックオンされないようにする
	isLockOnAvailable_ = false;
}

void Titan::Update() {
	// 状態遷移
	state_->Update();

	// 各パーツの更新処理
	partsManager_->Update();

	followWorldTransform_.UpdateMatrix();
	// ワールドトランスフォームを更新
	object3d_->worldTransform.UpdateMatrix();

	ImGui::Begin("Titan");
	ImGui::DragFloat3("translate", &object3d_->worldTransform.translate.x, 0.1f, -100, 100);
	ImGui::End();
}

void Titan::Draw() {
	// 体
	object3d_->Draw();
	// 他のパーツ
	partsManager_->Draw();
}

void Titan::OnCollision(Collider* collider) {
	if (hp_ <= 0.0f) {
		isDead_ = true;
	}
	else {
		hp_ = hp_ - collider->GetDamage();
	}
}

void Titan::ChangeState(ITitanState* pState) {
	delete state_;
	state_ = pState;
}
