#include "TitleScene.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
#include "GlobalVariables.h"
#include "Lerp.h"
#include "Audio.h"

void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;
	input_ = Input::GetInstance();

	// カメラの初期化
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->SetTranslate(Vector3{ 0,0,0 });

	// テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture("", "uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("", "monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("", "circle.png");
	TextureManager::GetInstance()->LoadTexture("", "block.png");
	TextureManager::GetInstance()->LoadTexture("", "rostock_laage_airport_4k.dds");
	// srvの番号取得
	uvcheckerTexture_ = TextureManager::GetInstance()->GetSrvIndex("", "uvChecker.png");
	monsterBallTexture_ = TextureManager::GetInstance()->GetSrvIndex("", "monsterBall.png");
	particleTexture_ = TextureManager::GetInstance()->GetSrvIndex("", "circle.png");
	blockTexture_ = TextureManager::GetInstance()->GetSrvIndex("", "block.png");
	ddsTexture_ = TextureManager::GetInstance()->GetSrvIndex("", "rostock_laage_airport_4k.dds");

	/// モデル読み込み
	/// 骨とアニメーションあり 
	ModelManager::GetInstance()->LoadModel("SimpleSkin", "simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("Human", "walk.gltf");
	ModelManager::GetInstance()->LoadModel("Human", "sneakWalk.gltf");

	ModelManager::GetInstance()->LoadModel("", "testPlane.gltf");
	ModelManager::GetInstance()->LoadModel("AnimatedCube", "AnimatedCube.gltf");

	// objモデル
	ModelManager::GetInstance()->LoadModel("", "axis.obj");
	ModelManager::GetInstance()->LoadModel("", "block.obj");

	//// 平面(骨とアニメーションあり)
	//plane_[0] = std::make_unique<Object3D>();
	//plane_[0]->Initialize();
	//plane_[0]->SetModel("SimpleSkin", "simpleSkin.gltf");
	//plane_[0]->SetCamera(camera_.get());
	//plane_[0]->SetAnimName("SimpleSkin");
	//plane_[0]->worldTransform.transform.translate = { 0,0,5 };
	//plane_[0]->worldTransform.transform.rotate = { 0,3.14f,0 };
	//// 平面(スケルトンなしのgltf)
	//plane_[1] = std::make_unique<Object3D>();
	//plane_[1]->Initialize();
	//plane_[1]->SetModel("", "testPlane.gltf");
	//plane_[1]->SetCamera(camera_.get());
	//plane_[1]->SetAnimName("NonAnim");
	//plane_[1]->worldTransform.transform.translate = { 0,-2,10 };
	//plane_[1]->worldTransform.transform.rotate = { -1.57f,0,0 };

	// 人間(スケルトンありのgltf)
	human_[0] = std::make_unique<Object3D>();
	human_[0]->Initialize();
	human_[0]->SetModel("Human", "walk.gltf");
	human_[0]->SetAnimName("Walk");
	human_[0]->SetCamera(camera_.get());

	human_[0]->model_->materialData_->color = { 1,1,1,0.8f };
	human_[0]->worldTransform.transform.translate = { 2,0,5 };
	//human_[1] = std::make_unique<Object3D>();
	//human_[1]->Initialize();
	//human_[1]->SetModel("Human", "sneakWalk.gltf");
	//human_[1]->SetCamera(camera_.get());
	//human_[1]->SetAnimName("SneakWalk");
	//human_[1]->worldTransform.transform.translate = { 2,-2,5 };

	//human_[0]->AddAnimation(human_[1]->model_);

	//// 
	//box_[0] = std::make_unique<Object3D>();
	//box_[0]->Initialize();
	//box_[0]->SetModel("AnimatedCube", "AnimatedCube.gltf");
	//box_[0]->SetCamera(camera_.get());
	//box_[0]->SetAnimName("Animation");
	//box_[0]->worldTransform.transform.translate = { -4,2,10 };
	//// objの箱
	//box_[1] = std::make_unique<Object3D>();
	//box_[1]->Initialize();
	//box_[1]->SetModel("", "block.obj");
	//box_[1]->SetCamera(camera_.get());
	//box_[1]->SetAnimName("NonAnim");
	//box_[1]->worldTransform.transform.translate = { -4,-2,10 };


	// 3D線(obj読み込み)
	axis_ = std::make_unique<Object3D>();
	axis_->Initialize();
	axis_->SetModel("", "axis.obj");
	axis_->SetCamera(camera_.get());
	axis_->worldTransform.transform.translate = { -2,0,5 };

	// 自機
	player_ = std::make_unique<Player>();
	player_->Initialize(camera_.get());

	// パーティクル
	particle_ = std::make_unique<Particles>();
	particle_->Initialize();
	particle_->SetCamera(camera_.get());
	particle_->SetEmitterCount(20);
	particle_->SetEmitterFrequency(0.1f);

	//// 音の読み込み
	//bgm_[0] = Audio::GetInstance()->SoundLoadWave("engine/resources/fanfare.wav");
	//bgm_[1] = Audio::GetInstance()->SoundLoadWave("engine/resources/fanfare.wav");
	//bgm_[2] = Audio::GetInstance()->SoundLoadWave("engine/resources/fanfare.wav");
	//bgm_[3] = Audio::GetInstance()->SoundLoadWave("engine/resources/fanfare.wav");
	//for (int i = 0; i < 4; i++) {
	//	//Audio::GetInstance()->SoundPlayWave(bgm_[i]);
	//}

	// Blender
	//LoadJSONFile("sample_map.json");
	//for (Object3D* object : levelObjects_) {
	//	object->SetCamera(camera_.get());
	//}

	cube_ = std::make_unique<Cube>();
	cube_->SetCamera(camera_.get());
	cube_->SetScale(Vector3{ 10,10,10 });
	cube_->SetPosition(Vector3{ 0,0,10 });
}

void TitleScene::Update() {
	particle_->Update();
	player_->Update();
#pragma region パーティクル以外の処理
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		sceneNum = GAME_SCENE;
	}

#ifdef _DEBUG
	ImGui::Begin("Current Scene");
	ImGui::Text("TITLE");
	ImGui::Text("keyInfo\n1:scene change");
	ImGui::End();
	human_[0]->ImGuiParameter("Human");
	//box_[0]->ImGuiParameter("AnimCube");
#endif
#pragma endregion
}

void TitleScene::Draw() {
	//axis_->Draw(uvcheckerTexture_);
	//human_[0]->Draw();
	//for (int i = 0; i < 2; i++) {
	//	plane_[i]->Draw(uvcheckerTexture_);
	//	box_[i]->Draw(uvcheckerTexture_);
	//	human_[i]->Draw(uvcheckerTexture_);
	//}

	for (Object3D* object : levelObjects_) {
		object->Draw(uvcheckerTexture_);
	}

	player_->Draw(uvcheckerTexture_);
	cube_->Draw(ddsTexture_);

	//particle_->Draw(uvcheckerTexture_);
}

void TitleScene::Finalize() {

}