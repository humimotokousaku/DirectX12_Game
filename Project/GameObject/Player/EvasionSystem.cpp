#include "EvasionSystem.h"
#include "Player.h"
#include "Utility.h"

void EvasionSystem::Initialize(Player* player, Camera* camera, Model* model) {
	gameTimer_ = GameTimer::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	// 残像オブジェクトに貼るテクスチャ
	afterImageTexture_ = textureManager_->GetSrvIndex("Textures", "Bob_Red.png");

	player_ = player;

	// 自機の残像オブジェクトを作成
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i] = std::make_unique<Object3D>();
		afterImageObject3d_[i]->Initialize();
		afterImageObject3d_[i]->SetModel(model);
		afterImageObject3d_[i]->SetCamera(camera);
		afterImageObject3d_[i]->worldTransform.scale = { 0.5f,0.5f,0.5f };
		afterImageObject3d_[i]->worldTransform.UpdateMatrix();
		// 透明にする
		afterImageObject3d_[i]->SetColor(Vector4{ 0.2f,0.2f,0.2f,0.3f });
		// 表示をしない
		afterImageObject3d_[i]->SetIsActive(false);
	}

	// ジャスト回避システム
	justEvasionSystem_ = std::make_unique<JustEvasionSystem>();
	justEvasionSystem_->Initialize();
	justEvasionSystem_->SetPlayer(player_);
	justEvasionSystem_->SetCamera(camera);

	// ジャスト回避時の文字
	justSprite_ = std::make_unique<Sprite>();
	justSprite_->Initialize("Textures/UI", "guide_JUST.png");
	justSprite_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2.0f, 64.0f });
	justSprite_->isActive_ = false;
	PostEffectManager::GetInstance()->AddSpriteList(justSprite_.get());
	// 初めてジャスト回避するときのUI
	firstJustSprite_ = std::make_unique<Sprite>();
	firstJustSprite_->Initialize("Textures/UI", "guide_pad_A.png");
	firstJustSprite_->SetSize(Vector2{ 64,64 });
	firstJustSprite_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2.0f, 200.0f });
	firstJustSprite_->isActive_ = false;
	PostEffectManager::GetInstance()->AddSpriteList(firstJustSprite_.get());

	// ジャスト判定
	justEvasionCollider_ = std::make_unique<Collider>();
	justEvasionCollider_->SetCollisionPrimitive(kCollisionOBB);
	//justEvasionCollider_->SetCollisionAttribute(kCollisionAttributePlayer);
	//justEvasionCollider_->SetCollisionMask(~kCollisionAttributePlayer);
	justEvasionCollider_->SetCollisionAttribute(kCollisionAttributeJustEvasion);
	justEvasionCollider_->SetCollisionMask(~kCollisionAttributeJustEvasion);
	justEvasionCollider_->SetOBBLength(Vector3{ 1.0f,1.0f,37.0f });
	justEvasionCollider_->SetOnCollision(std::bind(&EvasionSystem::JustOnCollision, this, std::placeholders::_1));
	justEvasionCollider_->SetIsActive(true);

	// ジャスト回避時のパーティクル
	justEvasionParticle_ = std::make_unique<Particles>();
	justEvasionParticle_->Initialize(player_->GetWorldPosition());
	justEvasionParticle_->SetCamera(camera);
	justEvasionParticle_->SetEmitterParent(player_->GetWorldTransform());
	justEvasionParticle_->SetParticleUpdate(std::bind(&EvasionSystem::JustParticleUpdate, this, std::placeholders::_1));
	// 発生頻度
	justEvasionParticle_->SetEmitterFrequency(1.0f / 240.0f);
	// 一度に発生する個数
	justEvasionParticle_->SetEmitterCount(40);
	// パーティクル一粒の詳細設定
	justEvasionParticle_->SetEmitterSpawnCount(1);
	justEvasionParticle_->randomScaleLimit = { 0.1f,0.3f };
	justEvasionParticle_->randomTranslateLimit = { -1.0f,1.0f };
	justEvasionParticle_->randomVelLimit[0] = { -2.0f,2.0f };
	justEvasionParticle_->randomVelLimit[1] = { -2.0f,2.0f };
	justEvasionParticle_->randomVelLimit[2] = { -2.0f,2.0f };
	justEvasionParticle_->randomColorLimit = { 0.0f,0.0f };
	justEvasionParticle_->randomLifeTimeLimit = { 10.1f,20.2f };
	justEvasionParticle_->particle_.color = { 0.0f,0.0f,1.0f,1.0f };

	// 回避時の移動速度のイージング
	evasionSpeedAnim_.SetAnimData(&evasion_.moveSpeed, kMaxEvasionMoveSpeed, Vector2{ 0.0f,0.0f }, kMaxEvasionFrame, Easings::EaseOutExpo);
	// 回避時の回転速度のイージング
	evasionRotSpeedAnim_.SetAnimData(&evasion_.rotVel, Vector3{ 0.0f,0.0f, 0.0f }, Vector3{ 0.0f, 0.0f ,(2.0f * (float)std::numbers::pi) * 4.0f }, 40, Easings::EaseOutExpo);
	// 残像のα値のアニメーション
	evasionAlphaAnims_.resize(afterImageObject3d_.size());
	evasion_.alphas.resize(evasionAlphaAnims_.size());
	for (int i = 0; i < evasionAlphaAnims_.size(); i++) {
		evasionAlphaAnims_[i].SetAnimData(&evasion_.alphas[i], 1.0f, 0.0f, 20, Easings::EaseOutExpo);
	}
}

void EvasionSystem::Update(float rotateY, float rotateX, Vector3& moveVel) {
	// ジャスト回避の更新処理
	JustEvasion();

	// 回避時の回転のイージングを更新
	evasionRotSpeedAnim_.Update();

	// ジャスト回避時の無敵時間更新
	InvisibleUpdate();

	// 回避中でなければ終了
	if (!evasion_.isActive) {
		// 移動方向に合わせて回避方向を指定
		Vector2 direction = { Utility::Sign(rotateY),Utility::Sign(-rotateX) };
		if (direction.x != 0.0f) {
			evasion_.direction.x = direction.x;
		}
		evasion_.direction.y = direction.y;
		return;
	}

#pragma region 回避速度の算出 
	// 回避速度のイージング開始
	evasionSpeedAnim_.SetIsStart(true);
	// 回避速度のイージングを更新
	evasionSpeedAnim_.Update();
	// 回避速度を正規化
	evasion_.moveSpeed *= Normalize(evasion_.direction) * 1.3f;
	moveVel.x += evasion_.moveSpeed.x * gameTimer_->GetTimeScale();
	moveVel.y += evasion_.moveSpeed.y * 0.5f * gameTimer_->GetTimeScale();
#pragma endregion

#pragma region 残像の処理
	// 残像を出す
	const int division = (int)afterImageObject3d_.size();
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		if ((int)evasion_.curretFrame % division == i) {
			// α値のイージング開始
			evasionAlphaAnims_[i].SetIsStart(true);
			// 残像オブジェクトを表示
			afterImageObject3d_[i]->SetIsActive(true);
			afterImageObject3d_[i]->worldTransform.translate = player_->GetWorldTransform()->translate;
			afterImageObject3d_[i]->worldTransform.rotate = player_->GetRotation();
		}
	}
	// 残像のα値のアニメーションを更新
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		evasionAlphaAnims_[i].Update();
		afterImageObject3d_[i]->SetAlpha(evasion_.alphas[i]);
	}
#pragma endregion

	// 時間を進める
	evasion_.curretFrame -= gameTimer_->GetTimeScale();
	// ジャスト猶予時間を進める
	evasion_.justFrame--;

	// イージングが終了したら初期化
	if (evasionSpeedAnim_.GetIsEnd()) {
		// 残像を消す
		for (int i = 0; i < afterImageObject3d_.size(); i++) {
			// アニメーション情報初期化
			evasionAlphaAnims_[i].ResetData();
			afterImageObject3d_[i]->SetIsActive(false);
		}
		// アニメーション情報初期化
		evasionSpeedAnim_.ResetData();
		evasion_.curretFrame = kMaxEvasionFrame;
		// ジャスト回避猶予時間
		evasion_.justFrame = kMaxJustEvasionFrame;
		evasion_.isActive = false;
	}
}

void EvasionSystem::Draw() {
	// 残像
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i]->Draw(afterImageTexture_);
	}

	// ジャスト時のパーティクル
	justEvasionParticle_->Draw(TextureManager::GetInstance()->GetSrvIndex("Level", "gray.png"));

	// ジャスト回避時の描画
	justEvasionSystem_->Draw();
}

void EvasionSystem::DrawUI() {
	// JUSTスプライト
	if (firstJustState_ == kFirstJust) {
		firstJustSprite_->isActive_ = true;
	}
	else {
		firstJustSprite_->isActive_ = false;
	}
}

void EvasionSystem::JustColliderUpdate() {
	// ジャスト判定を進める
	if (!evasion_.isActive) {
		justEvasionCollider_->worldTransform.translate = player_->GetWorldPosition();
	}
	else {
		justEvasionCollider_->worldTransform.translate.z = player_->GetWorldPosition().z;
	}
	justEvasionCollider_->worldTransform.UpdateMatrix();
	// 何も当たっていないとき補助時間をリセット
	if (!justEvasionCollider_->GetIsOnCollision()) { evasion_.justAssistFrame = kMaxJustEvasionAsisstFrame; }
}

void EvasionSystem::JustEvasion() {
	justEvasionSystem_->Update();

	// ジャスト回避演出時間を進める
	if (evasion_.justCurrentFrame > 0) {
		evasion_.justCurrentFrame--;
	}
	else if (evasion_.justCurrentFrame <= 0) {
		justSprite_->isActive_ = false;
		evasion_.isJust = false;
		justEvasionSystem_->SetIsActive(false);
	}

	// ジャスト回避中ではないならreturn
	if (!evasion_.isJust) { return; }

	// ジャスト時のパーティクル更新
	justEvasionParticle_->Update();

	// ジャスト回避の情報のみ初期化
	if (evasion_.justCurrentFrame <= 0) {
		evasion_.JustDataReset();
		justEvasionParticle_->SetEmitterSpawnLeft(1);
		return;
	}
}

bool EvasionSystem::IsJustEvasionFrame() {
	// ジャスト回避猶予時間のとき
	if (evasion_.isActive) {
		if (evasion_.justFrame >= 0) {
			return true;
		}
	}
	return false;
}

void EvasionSystem::JustOnCollision(Collider* collider) {
	// 自機との判定はとらない
	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) { return; }
	// ジャストの無敵中なら終了
	if (isInvinsible_) { return; }

	// 初めてジャスト回避を行う場合は説明を入れる
	if (firstJustState_ == kNone) {
		firstJustState_ = kFirstJust;
	}
	// ジャスト回避を補助するために時間を遅くする
	if (evasion_.justAssistFrame >= 0.0f && firstJustState_ == kEnd) {
		gameTimer_->SetTimeScale(0.1f);
	}

	evasion_.justAssistFrame--;

	// ジャスト回避猶予フレーム中
	if (IsJustEvasionFrame()) {
		if (evasion_.isJust) { return; }

		// ジャスト回避を起動
		evasion_.isJust = true;
		evasion_.justCurrentFrame = kJustEvasionAllFrame;
		// ジャストUIを表示
		justSprite_->isActive_ = true;

		justEvasionSystem_->SetIsActive(true);

		// 無敵状態にする
		invinsibleFrame_ = kMaxInvinsibleFrame;
		isInvinsible_ = true;
	}
}

void EvasionSystem::JustParticleUpdate(Particle& particle) {
	if (evasion_.justCurrentFrame >= 30) { return; }
	Vector3 distance = particle.transform.translate; // 左端 (x = 0) までの距離
	Vector3 force = /*-gravityStrength*/Multiply(-1.0f, (distance * distance + 1)); // 距離に反比例する力
	Vector3 acceleration = force / 10.0f; // F = ma の式から加速度を計算
	particle.vel += acceleration; // 加速度を速度に反映
}

void EvasionSystem::InvisibleUpdate() {
	if (!isInvinsible_) { return; }

	invinsibleFrame_--;

	if (invinsibleFrame_ <= 0) { isInvinsible_ = false; }
}

void EvasionSystem::Reset() {
	evasion_.Reset();
	justSprite_->isActive_ = false;
	// 残像オブジェクトの表示を消す
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i]->SetIsActive(false);
	}
}

void EvasionSystem::SetJustParameter() {
	// チュートリアルのジャスト回避
	if (firstJustState_ != kNone) {
		justEvasionCollider_->worldTransform.translate = player_->GetWorldPosition();
		evasion_.isActive = true;
		evasionRotSpeedAnim_.ResetData();
		evasionRotSpeedAnim_.SetIsStart(true);
		if (firstJustState_ == kFirstJust) { firstJustState_ = kEnd; }
	}
}
