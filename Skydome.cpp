#include "Skydome.h"

void Skydome::Initialize(Model* model, Camera* camera) {
	// 3Dオブジェクトの生成
	object3d_.Initialize();
	object3d_.SetModel(model);
	object3d_.SetCamera(camera);
}

void Skydome::Draw() {
	object3d_.Draw();
}