#include "BeamEnemy.h"
#include "BeamEnemyStateWait.h"
#include "EnemyManager.h" 

BeamEnemy::BeamEnemy() {
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
}

BeamEnemy::~BeamEnemy() {

}

void BeamEnemy::Initialize(Vector3 pos, Vector3 rotate, int id) {
#pragma region 体のオブジェクト
	object3d_->SetCamera(camera_);
	object3d_->SetModel(models_[0]);
	object3d_->worldTransform.translate = pos;
	object3d_->worldTransform.rotate = rotate;
	object3d_->worldTransform.UpdateMatrix();
	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	object3d_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	object3d_->collider->SetOnCollision(std::bind(&BeamEnemy::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(true);
#pragma endregion

#pragma region ビームオブジェクト生成
	beamObject_ = std::make_unique<Object3D>();
	beamObject_->Initialize();
	beamObject_->SetCamera(camera_);
	beamObject_->SetModel(models_[1]);
	beamObject_->worldTransform.translate.y = -50;
	beamObject_->worldTransform.scale = kDefaultBeamSize;
	beamObject_->worldTransform.UpdateMatrix();
	beamObject_->worldTransform.parent_ = &object3d_->worldTransform;
	// colliderの設定
	beamObject_->collider->SetCollisionPrimitive(kCollisionOBB);
	beamObject_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	beamObject_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	beamObject_->collider->SetOBBLength(beamObject_->worldTransform.scale);
	beamObject_->collider->SetDamage(30);
	beamObject_->collider->SetOnCollision(std::bind(&BeamEnemy::OnCollision, this, std::placeholders::_1));
	beamObject_->collider->SetIsActive(true);
#pragma endregion

	// 状態遷移
	state_ = new BeamEnemyStateWait(this, player_);
	state_->Initialize();

	// 体力
	hp_ = 100;

	// 管理番号
	id_ = id;
}

void BeamEnemy::Update() {
	if (hp_ <= 0.0f) {
		isDead_ = true;
	}

	// Catmull-Romスプライン関数で補間された位置を取得
	Vector3 pos{};
	pos = Lerps::CatmullRomSpline(controlPoints_, t_);
	object3d_->worldTransform.translate = pos;
	// 進行度を進める
	t_ += kMoveSpeed * GameTimer::GetInstance()->GetTimeScale();

	// ビームを回転させる
	beamObject_->worldTransform.rotate.y += 0.1f * GameTimer::GetInstance()->GetTimeScale();

	object3d_->worldTransform.UpdateMatrix();
}

void BeamEnemy::Draw() {
	// 体
	object3d_->Draw();

	// ビーム
	beamObject_->Draw();
}

void BeamEnemy::ChangeState(IBeamEnemyState* pState) {
	delete state_;
	state_ = pState;
}

void BeamEnemy::OnCollision(Collider* collider) {
	// 自機陣営に当たった場合のみダメージを受ける
	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		// HPを減らす
		hp_ = hp_ - collider->GetDamage();

		// 被弾時のパーティクルを生成
		CreateHitParticle();
	}
}