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

#pragma region 読み込み
	// テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture("uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("circle.png");
	TextureManager::GetInstance()->LoadTexture("block.png");
	TextureManager::GetInstance()->LoadTexture("rostock_laage_airport_4k.dds");
	for (int i = 0; i < numbersTexture_.size(); i++) {
		TextureManager::GetInstance()->LoadTexture(std::to_string(i) + ".png");
	}
	// srvの番号取得
	uvcheckerTexture_ = TextureManager::GetInstance()->GetSrvIndex("uvChecker.png");
	monsterBallTexture_ = TextureManager::GetInstance()->GetSrvIndex("monsterBall.png");
	particleTexture_ = TextureManager::GetInstance()->GetSrvIndex("circle.png");
	blockTexture_ = TextureManager::GetInstance()->GetSrvIndex("block.png");
	ddsTexture_ = TextureManager::GetInstance()->GetSrvIndex("", "rostock_laage_airport_4k.dds");
	whiteTexture_ = TextureManager::GetInstance()->GetSrvIndex("DefaultTexture","white.png");
	for (int i = 0; i < numbersTexture_.size(); i++) {
		numbersTexture_[i] = TextureManager::GetInstance()->GetSrvIndex(std::to_string(i) + ".png");
	}

	/// モデル読み込み
	// 骨とアニメーションあり 
	ModelManager::GetInstance()->LoadModel("SimpleSkin", "simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("Human", "walk.gltf");
	ModelManager::GetInstance()->LoadModel("Human", "sneakWalk.gltf");
	ModelManager::GetInstance()->LoadModel("", "testPlane.gltf");
	ModelManager::GetInstance()->LoadModel("AnimatedCube", "AnimatedCube.gltf");
	// objモデル
	ModelManager::GetInstance()->LoadModel("", "axis.obj");
	ModelManager::GetInstance()->LoadModel("", "block.obj");

	// 音の読み込み
	//bgm_[0] = Audio::GetInstance()->SoundLoadWave("engine/resources/fanfare.wav");
	//bgm_[1] = Audio::GetInstance()->SoundLoadWave("engine/resources/fanfare.wav");
	//bgm_[2] = Audio::GetInstance()->SoundLoadWave("engine/resources/fanfare.wav");
	//bgm_[3] = Audio::GetInstance()->SoundLoadWave("engine/resources/fanfare.wav");
	for (int i = 0; i < 1; i++) {
		//Audio::GetInstance()->SoundPlayWave(bgm_[i], 1.0f, 1.0f, 1.0f);
	}

	// Blender
	LoadJSONFile("GameMap_00.json");
	for (Object3D* object : levelObjects_) {
		object->SetCamera(camera_.get());
		object->SetIsLighting(false);
	}
	// カメラレールの生成
	railCamera_ = std::make_unique<RailCamera>();
	//railCamera_->Initialize(controlPoints_, camera_.get());
#pragma endregion

#pragma region 3Dモデルの生成
	// 平面(骨とアニメーションあり)
	plane_[0] = std::make_unique<Object3D>();
	plane_[0]->Initialize();
	plane_[0]->SetModel("SimpleSkin", "simpleSkin.gltf");
	plane_[0]->SetCamera(camera_.get());
	plane_[0]->SetAnimName("SimpleSkin");
	plane_[0]->worldTransform.translate = { 0,0,5 };
	plane_[0]->worldTransform.rotate = { 0,3.14f,0 };
	plane_[0]->StartAnim("SimpleSkin");
	// 平面(スケルトンなしのgltf)
	plane_[1] = std::make_unique<Object3D>();
	plane_[1]->Initialize();
	plane_[1]->SetModel("", "testPlane.gltf");
	plane_[1]->SetCamera(camera_.get());
	plane_[1]->SetAnimName("NonAnim");
	plane_[1]->worldTransform.translate = { 0,-2,10 };
	plane_[1]->worldTransform.rotate = { -1.57f,0,0 };
	// 人間(スケルトンありのgltf)
	// Walk
	human_[0] = std::make_unique<Object3D>();
	human_[0]->Initialize();
	human_[0]->SetModel("Human", "walk.gltf");
	human_[0]->SetAnimName("Walk");
	human_[0]->SetCamera(camera_.get());
	human_[0]->SetColor({ 1,1,1,1 });
	human_[0]->worldTransform.translate = { 2,0,5 };
	human_[0]->SetIsLighting(true);
	//human_[0]->StartAnim("Walk");
	// sneakWalk
	human_[1] = std::make_unique<Object3D>();
	human_[1]->Initialize();
	human_[1]->SetModel("Human", "sneakWalk.gltf");
	human_[1]->SetCamera(camera_.get());
	human_[1]->SetAnimName("SneakWalk");
	human_[1]->worldTransform.translate = { 2,-2,5 };
	human_[1]->StartAnim("SneakWalk");
	// walkアニメーションにsneakWalkを追加
	human_[0]->AddAnimation(human_[1]->GetModel());

	// 
	box_[0] = std::make_unique<Object3D>();
	box_[0]->Initialize();
	box_[0]->SetModel("AnimatedCube", "AnimatedCube.gltf");
	box_[0]->SetCamera(camera_.get());
	box_[0]->SetAnimName("Animation");
	box_[0]->StartAnim("Animation");
	box_[0]->worldTransform.translate = { -4,2,10 };
	// objの箱
	box_[1] = std::make_unique<Object3D>();
	box_[1]->Initialize();
	box_[1]->SetModel("AnimatedCube", "AnimatedCube.gltf");
	box_[1]->SetCamera(camera_.get());
	//box_[1]->StartAnim(0);
	box_[1]->worldTransform.translate = { -4,-2,10 };

	// 3D線(obj読み込み)
	axis_ = std::make_unique<Object3D>();
	axis_->Initialize();
	axis_->SetModel("", "axis.obj");
	axis_->SetCamera(camera_.get());
	axis_->worldTransform.translate = { -2,0,5 };
	axis_->SetIsLighting(true);
	axis_->CheckVertex();
#pragma endregion

	// Skybox
	cube_ = std::make_unique<Cube>();
	cube_->SetCamera(camera_.get());
	cube_->SetScale(Vector3{ 100,100,100 });
	cube_->SetPosition(Vector3{ 0,0,10 });

	// UIのスプライトを作成
	guideUI_[0] = std::make_unique<Sprite>();
	guideUI_[0]->Initialize("Level", "gray.png");
	guideUI_[0]->SetSize(Vector2{ 1280,720 });
	guideUI_[0]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 2 });
	guideUI_[1] = std::make_unique<Sprite>();
	guideUI_[1]->Initialize("Textures/UI", "titleName.png");
	guideUI_[1]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 });
	guideUI_[2] = std::make_unique<Sprite>();
	guideUI_[2]->Initialize("Textures/UI", "guide_pad_A.png");
	guideUI_[2]->SetSize(Vector2{ 64,64 });
	guideUI_[2]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 * 3 });
}

void TitleScene::Update() {
	ImGui::ShowDemoWindow();
#pragma region カメラ
	camera_->Update();
	// 座標移動
	if (Input::GetInstance()->PressKey(DIK_RIGHT)) {
		camera_->worldTransform_.translate.x += 0.05f;
	}
	if (Input::GetInstance()->PressKey(DIK_LEFT)) {
		camera_->worldTransform_.translate.x -= 0.05f;
	}
	if (!Input::GetInstance()->PressKey(DIK_SPACE) && Input::GetInstance()->PressKey(DIK_UP)) {
		camera_->worldTransform_.translate.y += 0.05f;
	}
	else if (Input::GetInstance()->PressKey(DIK_SPACE) && Input::GetInstance()->PressKey(DIK_UP)) {
		camera_->worldTransform_.translate.z += 0.05f;
	}
	if (!Input::GetInstance()->PressKey(DIK_SPACE) && Input::GetInstance()->PressKey(DIK_DOWN)) {
		camera_->worldTransform_.translate.y -= 0.05f;
	}
	else if (Input::GetInstance()->PressKey(DIK_SPACE) && Input::GetInstance()->PressKey(DIK_DOWN)) {
		camera_->worldTransform_.translate.z -= 0.05f;
	}

	// 回転
	if (Input::GetInstance()->PressKey(DIK_W)) {
		camera_->worldTransform_.rotate.x += -0.01f;
	}
	if (Input::GetInstance()->PressKey(DIK_S)) {
		camera_->worldTransform_.rotate.x += 0.01f;
	}
	if (Input::GetInstance()->PressKey(DIK_A)) {
		camera_->worldTransform_.rotate.y += -0.01f;
	}
	if (Input::GetInstance()->PressKey(DIK_D)) {
		camera_->worldTransform_.rotate.y += 0.01f;
	}
#pragma endregion

	if (Input::GetInstance()->TriggerKey(DIK_1) || Input::GetInstance()->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		sceneNum = GAME_SCENE;
	}
	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		sceneNum = GAMEOVER_SCENE;
	}
	if (Input::GetInstance()->TriggerKey(DIK_3)) {
		sceneNum = GAMECLEAR_SCENE;
	}
	//if (Input::GetInstance()->TriggerKey(DIK_R)) {
	//	human_[0]->EndAnim();
	//}

#ifdef _DEBUG
	//human_[0]->ImGuiParameter("Human");
	//plane_[0]->ImGuiParameter("Plane");
	//box_[0]->ImGuiParameter("AnimCube");
	axis_->ImGuiParameter("axis");
#endif
}

void TitleScene::Draw() {
	//axis_->Draw(particleTexture_);
	for (int i = 0; i < 2; i++) {
		//plane_[i]->Draw(uvcheckerTexture_);
		//box_[i]->Draw(uvcheckerTexture_);
		//human_[i]->Draw(uvcheckerTexture_);
	}

	// Blenderで配置したオブジェクト
	for (Object3D* object : levelObjects_) {
		//object->Draw();
	}
	//railCamera_->MoveRouteDraw();
	//cube_->Draw(ddsTexture_);

	// UI
	for (int i = 0; i < 3; i++) {
		guideUI_[i]->Draw();
	}
}

void TitleScene::Finalize() {
	// 基底クラスの解放
	IScene::Finalize();
}