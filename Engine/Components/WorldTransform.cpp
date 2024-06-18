#include "WorldTransform.h"

void WorldTransform::Initialize() {
	matWorld_ = MakeIdentity4x4();
	CreateConstBuffer();
	Map();
	TransferMatrix();
}

void WorldTransform::CreateConstBuffer() {
	constBuff_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(ConstBufferDataWorldTransform));
}

void WorldTransform::Map() {
	constBuff_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
}

void WorldTransform::TransferMatrix() {
	constMap->matWorld = matWorld_;
	constMap->WorldInverseTranspose = Inverse(Transpose(matWorld_));
}

void WorldTransform::UpdateMatrix() {
	matWorld_ = MakeAffineMatrix(scale, rotate, translate);

	// 親子関係の計算
	if (parent_) {
		matWorld_ = Multiply(matWorld_, parent_->matWorld_);
	}

	TransferMatrix();
}
