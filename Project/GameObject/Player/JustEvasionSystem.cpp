#include "JustEvasionSystem.h"
#include "Player.h"

JustEvasionSystem::~JustEvasionSystem() {
	justParticles_.clear();
}

void JustEvasionSystem::Initialize() {
	// インスタンスを取得
	textureManager_ = TextureManager::GetInstance();
	postEffectManager_ = PostEffectManager::GetInstance();
	score_ = Score::GetInstance();

	justParticleTexture_ = textureManager_->GetSrvIndex("Textures/DefaultTexture", "white.png");
}

void JustEvasionSystem::Update() {
	// ジャスト回避した瞬間のみパーティクル生成
	if (!isPreActive_ && isActive_) {
		CreateJustParticle();
	}
	// ジャスト回避時のパーティクル更新
	JustParticleUpdate();

	// パーティクルが消える範囲
	CheckBox();

	isPreActive_ = isActive_;
}

void JustEvasionSystem::Draw() {
	for (JustParticleData& justParticle : justParticles_) {
		justParticle.particle->Draw();
	}
}

void JustEvasionSystem::JustParticleUpdate() {
	if (!isActive_) { return; }

	for (JustParticleData& justParticle : justParticles_) {
		justParticle.velocity = Lerps::ExponentialInterpolate(justParticle.velocity, Vector2{ 0.0f,0.0f }, 0.01f);
		// 弾ゲージにパーティクルを寄せる
		if (player_->GetEvasionData().justCurrentFrame <= 25) {
			// 弾ゲージとの方向ベクトル
			Vector2 directionVel = player_->GetBulletGauge().sprite->GetPos() - justParticle.pos;
			directionVel *= 0.01f;

			// 加速度
			justParticle.acceleration = directionVel;
			justParticle.velocity += justParticle.acceleration;
		}
		justParticle.pos += justParticle.velocity;
		// 座標に速度を加算
		justParticle.particle->SetPos(justParticle.pos);
	}
}

void JustEvasionSystem::CreateJustParticle() {
	// パーティクル数を指定
	justParticles_.resize(kMaxJustParticleNum);
	for (JustParticleData& justParticle : justParticles_) {
		justParticle.particle = std::make_unique<Sprite>();
		justParticle.particle->Initialize(justParticleTexture_);
		justParticle.particle->SetPos(ConvertWorld2Screen(player_->GetWorldPosition()));
		justParticle.particle->SetColor(Vector4{ 0.0f,0.0f,1.0f,1.0f });
		justParticle.pos = justParticle.particle->GetPos();
		justParticle.velocity = GenerateRandomVelocity(kJustParticleVelocityRange);
	}
}

void JustEvasionSystem::CheckBox() {
	for (JustParticleData& justParticle : justParticles_) {
		if (108.0f >= justParticle.pos.y && 512.0f >= justParticle.pos.x ) {
			if (!justParticle.particle->isActive_) { continue; }
			// 弾ゲージを増加
			player_->IncrementBulletGauge(1.0f);
			justParticle.particle->isActive_ = false;
			// スコア加算
			score_->AddScore(100);
		}
	}
}

Vector2 JustEvasionSystem::ConvertWorld2Screen(Vector3 worldPos) {
	Vector3 result = worldPos;
	if (result.x == 0 && result.y == 0 && result.z == 0) {
		result = { 0.000001f,0.000001f ,0.000001f };
	}
	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport =
		Multiply(camera_->GetViewProjection().matView, Multiply(camera_->GetViewProjection().matProjection, matViewport));
	// ワールド→スクリーン座標変換
	result = Transforms(result, matViewProjectionViewport);

	return Vector2(result.x, result.y);
}