#include "IObject3D.h"

void IObject3D::SetModel(const std::string& directoryPath, const std::string& filePath) {
	*model_ = *ModelManager::GetInstance()->GetModel(directoryPath, filePath);
	
	// material
	Material* materialData;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->enableLighting = true;
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = MakeIdentity4x4();
	model_->SetMaterialResource(materialResource);
	model_->SetMaterialData(materialData);

	// Dissolve
	DissolveDataForGPU* dissolveData;
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource;
	// Dissolveの情報を書き込む
	dissolveResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(DissolveDataForGPU)).Get();
	// 書き込むためのアドレスを取得
	dissolveResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&dissolveData));
	dissolveData->isActive = true;
	dissolveData->maskThreshold = 0.5f;
	model_->SetDissolveResource(dissolveResource);
	model_->SetDissolveData(dissolveData);

	Motion animation = model_->GetAnimation();
	// アニメーション
	animation_.push_back(animation);
	// スケルトン
	skeleton_ = model_->GetSkeleton();
	// スキンクラスタがあるなら作る
	if (model_->GetModelData().isSkinClusterData) {
		SkinCluster skinCluster = CreateSkinCluster(skeleton_, model_->GetModelData());
		skinCluster_.push_back(skinCluster);
	}
}

void IObject3D::SetModel(Model* model) {
	*model_ = *model;

	// material
	Material* materialData;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->enableLighting = true;
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = MakeIdentity4x4();
	model_->SetMaterialResource(materialResource);
	model_->SetMaterialData(materialData);

	// Dissolve
	DissolveDataForGPU* dissolveData;
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource;
	// Dissolveの情報を書き込む
	dissolveResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(DissolveDataForGPU)).Get();
	// 書き込むためのアドレスを取得
	dissolveResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&dissolveData));
	dissolveData->isActive = true;
	dissolveData->maskThreshold = 0.5f;
	model_->SetDissolveResource(dissolveResource);
	model_->SetDissolveData(dissolveData);

	Motion animation = model_->GetAnimation();
	// アニメーション
	animation_.push_back(animation);
	// スケルトン
	skeleton_ = model_->GetSkeleton();
	// スキンクラスタがあるなら作る
	if (model_->GetModelData().isSkinClusterData) {
		SkinCluster skinCluster = CreateSkinCluster(skeleton_, model_->GetModelData());
		skinCluster_.push_back(skinCluster);
	}
}
