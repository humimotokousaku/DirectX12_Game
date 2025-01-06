#include "TitleEvent.h"

void TitleEvent::Initialize() {
	textureManager_ = TextureManager::GetInstance();
	modelManager_ = ModelManager::GetInstance();
	levelManager_ = LevelManager::GetInstance();
	input_ = Input::GetInstance();
	postEffectManager_ = PostEffectManager::GetInstance();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	// 追従カメラの生成
	followCamera_ = std::make_unique<FollowCamera>();

#pragma region 読み込み
	// テクスチャの読み込み
	textureManager_->LoadTexture("Textures", "reticle.png");
	textureManager_->LoadTexture("Textures", "Bob_Red.png");
	textureManager_->LoadTexture("Textures", "Spitfire_Purple.png");

	// 使用するモデルの読み込み
	modelManager_->LoadModel("Models", "block.obj");
	modelManager_->LoadModel("Models", "Bob.obj");
	modelManager_->LoadModel("Models", "Spitfire.obj");
	modelManager_->LoadModel("Level", "tail.obj");
	modelManager_->LoadModel("Level", "skydome.obj");

	// Blender
	levelManager_->LoadJSONFile("TitleMap.json", followCamera_->GetCamera());
#pragma endregion

	// 自機の生成
	player_ = std::make_unique<Player>();
	// 自機モデル
	player_->AddModel("PlayerBody",modelManager_->GetModel("Models", "Bob.obj"));
	// 弾モデル
	player_->AddModel("PlayerBullet", modelManager_->GetModel("Models", "block.obj"));
	// 必要なクラスのアドレスをセットする
	player_->SetCamera(followCamera_->GetCamera());
	player_->Initialize();

	// 追従対象
	followObject_.Initialize();
	followObject_.translate.z = -0.01f;
	followObject_.rotate = kStartCameraRotate;
	followObject_.UpdateMatrix();

	// タイトルシーンからゲームシーンになるまでのカメラアニメーション
	gameStartAngleAnim_.SetAnimData(&followObject_.rotate, Vector3{ followObject_.rotate.x, followObject_.rotate.y, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, 50, Easings::EaseOutQuart);

	// 追従カメラの初期化
	followCamera_->Initialize(player_.get());
	followCamera_->SetParent(&followObject_);
	followCamera_->SetTargetOffset(kStartCameraOffset);

	// 天球の生成
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelManager_->GetModel("Level", "skydome.obj"),followCamera_->GetCamera());

	isActive_ = false;
	isEnd_ = false;
}

void TitleEvent::Update() {
	// タイトルシーンからゲームシーンに切り替わるまでの演出
	TitleSceneToGameSceneEffect();

	// カメラ演出
	CameraUpdate();

	followObject_.rotate.x = std::fmod(followObject_.rotate.x, (float)std::numbers::pi * 2.0f);
	followObject_.rotate.y = std::fmod(followObject_.rotate.y, (float)std::numbers::pi * 2.0f);
	followObject_.rotate.z = std::fmod(followObject_.rotate.z, (float)std::numbers::pi * 2.0f);
}

void TitleEvent::Draw() {
	// ブレンダーで配置したモデル
	levelManager_->Draw();

	// 天球
	skydome_->Draw();

	// 自機
	player_->Draw();
}

void TitleEvent::TitleSceneToGameSceneEffect() {
	if (!isActive_) { return; }

	gameStartAngleAnim_.SetIsStart(true);
	gameStartAngleAnim_.Update();

	// カメラを自機の背後に向けたら自機を飛ばす
	if (gameStartAngleAnim_.GetIsEnd()) {
		// ラジアルブラーをかける
		postEffectManager_->radialBlurData_.blurWidth = 0.05f;
		postEffectManager_->radialBlurData_.isActive = true;

		// 追従カメラの座標を固定する
		followCamera_->PositionLock(followObject_.translate);

		// 自機のいるところにカメラを向ける
		Vector3 toPlayer = player_->GetWorldPosition() - followObject_.GetWorldPosition();
		toPlayer = Normalize(toPlayer);
		velocity_ = Normalize(velocity_);
		velocity_ = Lerps::Slerp(velocity_, toPlayer, 1.0f) * GameTimer::GetInstance()->GetTimeScale();
		// Y軸周り角度(θy)
		followObject_.rotate.y = std::atan2(velocity_.x, velocity_.z);
		// 横軸方向の長さを求める
		float velocityXZ;
		velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
		// X軸周りの角度(θx)
		followObject_.rotate.x = std::atan2(-velocity_.y, velocityXZ);
		followObject_.UpdateMatrix();

		player_->TitleEffect(isEnd_);
		// 演出がすべて終了したら演出中フラグを切る
		if (isEnd_) { isActive_ = false; }
	}
}

void TitleEvent::CameraUpdate() {
	// 追従カメラ
	followCamera_->Update();

	// ゲーム開始演出中は処理しない
	if (isActive_) { return; }

	// カメラを回転させる
	followObject_.rotate.y += 0.01f;
	followObject_.UpdateMatrix();

	// アニメーション情報を更新
	gameStartAngleAnim_.SetFirstAnimData(&followObject_.rotate, Vector3{ followObject_.rotate.x, followObject_.rotate.y, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, 50, Easings::EaseOutQuart);
}
