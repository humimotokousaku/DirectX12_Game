#include "SceneTransition.h"

void SceneTransition::Initialize() {
	for (int i = 0; i < 3; i++) {
		sprite_[i].Initialize("DefaultTexture", "white.png");
		sprite_[i].SetSize(Vector2{ 1280.0f, 720.0f });
		sprite_[i].worldTransform_.translate.x = WinApp::kClientWidth_ / 2;
		sprite_[i].worldTransform_.translate.y = WinApp::kClientHeight_ / 2;
	}
	sprite_[0].SetColor(Vector4{ 0,1,0,1 });
	sprite_[1].SetColor(Vector4{ 0,0,1,1 });
	sprite_[2].SetColor(Vector4{ 0,0,0,1 });

	for (int i = 0; i < 3; i++) {
		spritePos_[i] = &sprite_[i].worldTransform_.translate;
	}

	//beginAnim_.SetAnimData(spritePos_[0], Vector3{ (float)-WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2, 0 }, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2, 0 }, 10, "sceneTransition_00", Easings::EaseInOutExpo);
	//beginAnim_.SetAnimData(spritePos_[0], Vector3{ (float)-WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2, 0 }, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2, 0 }, 10, "sceneTransition_01", Easings::EaseInOutExpo);
	//beginAnim_.SetAnimData(spritePos_[0], Vector3{ (float)-WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2, 0 }, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2, 0 }, 10, "sceneTransition_02", Easings::EaseInOutExpo);
}

void SceneTransition::Update() {

}

void SceneTransition::BeginEffect() {

}

void SceneTransition::EndEffect() {

}
