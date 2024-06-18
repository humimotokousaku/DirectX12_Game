#include "Camera.h"
#include "ImGuiManager.h"

Camera::~Camera() {

}

void Camera::Initialize() {
	CreateViewProjection();
	worldTransform_.Initialize();
}

void Camera::Update() {
	UpdateMatrix();
	worldTransform_.UpdateMatrix();

	//// 座標移動
	//if (Input::GetInstance()->PressKey(DIK_RIGHT)) {
	//	worldTransform_.translate.x += 0.01f;
	//}
	//if (Input::GetInstance()->PressKey(DIK_LEFT)) {
	//	worldTransform_.translate.x -= 0.01f;
	//}
	//if (!Input::GetInstance()->PressKey(DIK_SPACE) && Input::GetInstance()->PressKey(DIK_UP)) {
	//	worldTransform_.translate.y += 0.01f;
	//}
	//else if (Input::GetInstance()->PressKey(DIK_SPACE) && Input::GetInstance()->PressKey(DIK_UP)) {
	//	worldTransform_.translate.z += 0.01f;
	//}
	//if (!Input::GetInstance()->PressKey(DIK_SPACE) && Input::GetInstance()->PressKey(DIK_DOWN)) {
	//	worldTransform_.translate.y -= 0.01f;
	//}
	//else if (Input::GetInstance()->PressKey(DIK_SPACE) && Input::GetInstance()->PressKey(DIK_DOWN)) {
	//	worldTransform_.translate.z -= 0.01f;
	//}

	//// 回転
	//if (Input::GetInstance()->PressKey(DIK_W)) {
	//	worldTransform_.rotate.x += -0.01f;
	//}
	//if (Input::GetInstance()->PressKey(DIK_S)) {
	//	worldTransform_.rotate.x += 0.01f;
	//}
	//if (Input::GetInstance()->PressKey(DIK_A)) {
	//	worldTransform_.rotate.y += -0.01f;
	//}
	//if (Input::GetInstance()->PressKey(DIK_D)) {
	//	worldTransform_.rotate.y += 0.01f;
	//}
}

void Camera::CreateViewProjection() {
	CreateConstBuffer();
	Map();
	UpdateMatrix();
	TransferMatrix();
}

void Camera::CreateConstBuffer() {
	viewProjection_.constBuff_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(ConstBufferDataViewProjection));
}

void Camera::Map() {
	viewProjection_.constBuff_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&viewProjection_.constMap));
}

void Camera::UpdateMatrix() {
	UpdateViewMatrix();
	UpdateProjectionMatrix();
	TransferMatrix();
}

void Camera::TransferMatrix() {
	viewProjection_.constMap->view = viewProjection_.matView;
	viewProjection_.constMap->projection = viewProjection_.matProjection;
}

void Camera::UpdateViewMatrix() {
	Matrix4x4 cameraMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	viewProjection_.matView = Inverse(cameraMatrix);
}

void Camera::UpdateProjectionMatrix() {
	viewProjection_.matProjection = MakePerspectiveFovMatrix(viewProjection_.fovAngleY, viewProjection_.aspectRatio, viewProjection_.nearZ, viewProjection_.farZ);
}