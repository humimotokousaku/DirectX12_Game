#include "TitleScene.h"
#include "ImGuiManager.h"
#include "ModelManager.h"


void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;

	// カメラ生成
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->worldTransform_.translate = { 0.0f,0.0f,-10.0f };

	// ddsテクスチャ
	TextureManager::GetInstance()->LoadTexture("uvChecker.dds");
	ddsTexture_ = TextureManager::GetInstance()->GetSrvIndex("uvChecker.dds");

	// モデルの読みこみ
	ModelManager::GetInstance()->LoadModel("block.obj");

	// モデル生成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_.get());
	object3d_->SetModel(ModelManager::GetInstance()->GetModel("block.obj"));
}

void TitleScene::Update() {
	camera_->Update();
}

void TitleScene::Draw() {
	object3d_->Draw(ddsTexture_);
}

void TitleScene::Finalize() {

}