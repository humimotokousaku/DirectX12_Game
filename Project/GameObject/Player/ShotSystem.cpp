#include "ShotSystem.h"
#include "GameSystem.h"

void ShotSystem::Initialize(Player* player, Camera* camera, std::list<IEnemy*> enemys, GameSystem* gameSystem, Model* model) {
	player_ = player;
	camera_ = camera;
	enemys_ = enemys;
	gameSystem_ = gameSystem;
	model_ = model;
}

void ShotSystem::Update() {
	//lockOnDatas_.resize(multiLockOnDatas_.size());
	//for (int i = 0; i < multiLockOnDatas_.size(); i++) {
	//	lockOnDatas_[i].enemyData = multiLockOnDatas_[i].worldTransform;
	//}

	Attack();
}

void ShotSystem::Attack() {
	// Rトリガーを押していたら
	//if (joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER || Input::GetInstance()->PressKey(DIK_SPACE)) {
		// 弾ゲージが0以外で発射クールタイムが0のとき
	//if (bulletInterval_ <= 0 && bulletGauge_.value >= kDecrementBulletGauge) {
	//for (LockOnData& lockOnPos : lockOnDatas_) {
	//	if (lockOnPos.isActive) { continue; }
	//	lockOnPos.isActive = true;

	//	// 弾の速度
	//	const float kBulletSpeed = 5.0f;
	//	Vector3 velocity(0, 0, kBulletSpeed);
	//	// 自機の座標
	//	Vector3 worldPos = player_->GetWorldPosition();

	//	Matrix4x4 m = player_->GetWorldTransform()->matWorld_;
	//	// 速度ベクトルを自機の向きに合わせて回転させる
	//	velocity = TransformNormal(velocity, m);
	//	// 自機から照準オブジェクトへのベクトル
	//	Vector3 targetPos = lockOnPos.enemyData->GetWorldPosition();
	//	velocity = targetPos - worldPos;
	//	velocity = Normalize(velocity);
	//	velocity *= kBulletSpeed;

	//	// 弾を生成し、初期化
	//	PlayerBullet* newBullet = new PlayerBullet();
	//	newBullet->SetCamera(camera_);
	//	newBullet->Initialize(model_, worldPos, velocity, lockOnPos.enemyData);
	//	// 弾を登録
	//	gameSystem_->AddPlayerBullet(newBullet);

	//	// 弾の発射間隔リセット
	//	//bulletInterval_ = kBulletInterval;

	//	// 弾ゲージを減少させる
	//	//bulletGauge_.value -= kDecrementBulletGauge;

	//	// 音の再生
	//	//audio_->SoundPlayWave(shotSE_, false, 0.25f);

	//	if (lockOnPos.isActive) { break; }
	//}
	//}
	//}

	//bulletInterval_--;
	//// 0未満にならないようにする
	//bulletInterval_ = std::clamp<int>(bulletInterval_, 0, kBulletInterval);
}