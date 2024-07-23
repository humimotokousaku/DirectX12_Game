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
	viewProjection_.matProjection = MakePerspectiveFovMatrix(viewProjection_.fovAngleY * std::numbers::pi / 180.0f, viewProjection_.aspectRatio, viewProjection_.nearZ, viewProjection_.farZ);
}