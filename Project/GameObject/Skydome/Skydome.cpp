#include "Skydome.h"

void Skydome::Initialize(Model* model, Camera* camera, const WorldTransform* parent) {
	// 3Dオブジェクトの生成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetModel(model);
	object3d_->SetCamera(camera);
	object3d_->worldTransform.scale = { 1000.0f,1000.0f ,1000.0f };
	object3d_->worldTransform.parent_ = parent;
	// ライティングをしない
	object3d_->SetIsLighting(false);
	object3d_->SetColor(Vector4{ 0.5f, 0.5f, 0.5f,1.0f });
	// 当たり判定をoff
	object3d_->collider->SetIsActive(false);
}

void Skydome::Draw() {
	object3d_->Draw();
}