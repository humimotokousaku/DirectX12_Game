#include "IObject3D.h"

void IObject3D::SetModel(const std::string& directoryPath, const std::string& filePath) {
	*model_ = *ModelManager::GetInstance()->GetModel(directoryPath, filePath);
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Material* materialData;
	materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	model_->SetMaterialResource(materialResource);
	materialData->enableLighting = true;
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = MakeIdentity4x4();
	model_->materialData_ = materialData;

	Motion animation = model_->animation_;
	// アニメーション
	animation_.push_back(animation);
	// スケルトン
	skeleton_ = model_->skeleton_;
	// スキンクラスタがあるなら作る
	if (model_->GetModelData().isSkinClusterData) {
		SkinCluster skinCluster = CreateSkinCluster(skeleton_, model_->GetModelData());
		skinCluster_.push_back(skinCluster);
	}
}

void IObject3D::SetModel(Model* model) {
	*model_ = *model;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Material* materialData;
	materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	model_->SetMaterialResource(materialResource);
	materialData->enableLighting = true;
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = MakeIdentity4x4();
	model_->materialData_ = materialData;

	Motion animation = model_->animation_;
	// アニメーション
	animation_.push_back(animation);
	// スケルトン
	skeleton_ = model_->skeleton_;
	// スキンクラスタがあるなら作る
	if (model_->GetModelData().isSkinClusterData) {
		SkinCluster skinCluster = CreateSkinCluster(skeleton_, model_->GetModelData());
		skinCluster_.push_back(skinCluster);
	}
}
