#include "Model.h"
#include "ImGuiManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "TextureManager.h"
#include "PipelineManager.h"
#include  "ModelManager.h"
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::~Model() {
	for (int i = 0; i < skeletonLine_.size(); i++) {
		delete skeletonLine_[i];
	}
}

void Model::Initialize(const std::string& directoryPath, const std::string& filename) {
	// エンジン機能のインスタンスを入れる
	dxCommon_ = DirectXCommon::GetInstance();
	texManager_ = TextureManager::GetInstance();

	// モデルの読み込み
	modelData_ = LoadModelFile(directoryPath, filename);
	// アニメーションデータを読み込む
	animation_ = LoadAnimationFile(directoryPath, filename);
	// スケルトンデータを作成
	skeleton_ = CreateSkeleton(modelData_.rootNode);
	// スキンクラスタを作成
	skinCluster_ = CreateSkinCluster(skeleton_, modelData_);

	// モデルファイルと同じ階層にテクスチャがない場合、デフォルトのテクスチャが入るようにする
	texHandle_ = 1;
	// テクスチャ読み込み 
	texManager_->LoadTexture(modelData_.material.textureFilePath);
	texHandle_ = texManager_->GetSrvIndex(modelData_.material.textureFilePath);

	// 頂点データのリソース作成
	CreateVertexResource();
	CreateVertexBufferView();
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	// インデックスのリソース作成
	indexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * modelData_.indices.size();
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	// 書き込むためのアドレスを取得
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
	std::memcpy(mappedIndex_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());

	// マテリアルデータのリソース作成
	CreateMaterialResource();

	// カメラ
	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));

	// Lightingするか
	materialData_->enableLighting = false;
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	// uvTransform行列の初期化
	materialData_->uvTransform = MakeIdentity4x4();
	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
}

void Model::Initialize() {
	// エンジン機能のインスタンスを入れる
	dxCommon_ = DirectXCommon::GetInstance();
	texManager_ = TextureManager::GetInstance();

	//// モデルの読み込み
	//modelData_ = LoadModelFile(filename);
	//// アニメーションデータを読み込む
	//animation_ = LoadAnimationFile(filename);
	//// スケルトンデータを作成
	//skeleton_ = CreateSkeleton(modelData_.rootNode);
	//// スキンクラスタを作成
	//skinCluster_ = CreateSkinCluster(skeleton_, modelData_);

	// モデルファイルと同じ階層にテクスチャがない場合、デフォルトのテクスチャが入るようにする
	texHandle_ = 1;
	// テクスチャ読み込み 
	texManager_->LoadTexture(modelData_.material.textureFilePath);
	texHandle_ = texManager_->GetSrvIndex(modelData_.material.textureFilePath);

	// 頂点データのリソース作成
	CreateVertexResource();
	CreateVertexBufferView();
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	// インデックスのリソース作成
	indexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * modelData_.indices.size();
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	// 書き込むためのアドレスを取得
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
	std::memcpy(mappedIndex_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());

	// マテリアルデータのリソース作成
	CreateMaterialResource();

	// カメラ
	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));

	// Lightingするか
	materialData_->enableLighting = false;
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	// uvTransform行列の初期化
	materialData_->uvTransform = MakeIdentity4x4();
	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
}

void Model::Initialize(const std::string& filename) {
	// エンジン機能のインスタンスを入れる
	dxCommon_ = DirectXCommon::GetInstance();
	texManager_ = TextureManager::GetInstance();

	// モデルの読み込み
	modelData_ = LoadModelFile(filename);
	// アニメーションデータを読み込む
	animation_ = LoadAnimationFile(filename);
	// スケルトンデータを作成
	skeleton_ = CreateSkeleton(modelData_.rootNode);
	// スキンクラスタを作成
	skinCluster_ = CreateSkinCluster(skeleton_, modelData_);

	// モデルファイルと同じ階層にテクスチャがない場合、デフォルトのテクスチャが入るようにする
	texHandle_ = 1;
	// テクスチャ読み込み 
	texManager_->LoadTexture(modelData_.material.textureFilePath);
	texHandle_ = texManager_->GetSrvIndex(modelData_.material.textureFilePath);

	// 頂点データのリソース作成
	CreateVertexResource();
	CreateVertexBufferView();
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	// インデックスのリソース作成
	indexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * modelData_.indices.size();
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	// 書き込むためのアドレスを取得
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
	std::memcpy(mappedIndex_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());

	// マテリアルデータのリソース作成
	CreateMaterialResource();

	// カメラ
	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));

	// Lightingするか
	materialData_->enableLighting = false;
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	// uvTransform行列の初期化
	materialData_->uvTransform = MakeIdentity4x4();
	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
}

void Model::Draw(const ViewProjection& viewProjection, uint32_t textureHandle) {
	if (modelData_.isSkinClusterData) {
		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			vertexBufferView_,
			skinCluster_.influenceBufferView
		};
		dxCommon_->GetCommandList()->IASetVertexBuffers(0, 2, vbvs); // VBVを設定
		// MatrixPalette
		SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(8, skinCluster_.srvIndex);
	}
	else {
		dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	}
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	/// CBVの設定

	// viewProjection
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraPosResource_.Get()->GetGPUVirtualAddress());

	// ライティング
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, SpotLight::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());
	/// DescriptorTableの設定
	// texture
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, textureHandle);

	// material
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());

	dxCommon_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
}

void Model::Draw(const ViewProjection& viewProjection) {
	if (modelData_.isSkinClusterData) {
		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			vertexBufferView_,
			skinCluster_.influenceBufferView
		};
		dxCommon_->GetCommandList()->IASetVertexBuffers(0, 2, vbvs); // VBVを設定
		// MatrixPalette
		SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(8, skinCluster_.srvIndex);
	}
	else {
		dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	}
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	/// CBVの設定

	// viewProjection
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraPosResource_.Get()->GetGPUVirtualAddress());

	/// DescriptorTableの設定
	// texture
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, texHandle_);

	// material
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// ライティング
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, SpotLight::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
}

void Model::AdjustParameter() {
	ImGui::Begin("Model");
	//ImGui::DragFloat3("translation", worldtrans)
	ImGui::End();
}

void Model::SkeletonLineInit() {
	// メモリーを確保
	skeletonLine_.resize(skeleton_.joints.size());

	// 初期化
	for (int i = 0; i < skeletonLine_.size(); i++) {
		skeletonLine_[i] = new Line();
		skeletonLine_[i]->Initialize();
	}

	// jointの座標を代入
	for (int i = 0; i < skeletonLine_.size(); i++) {
		// 親子関係のあるノードを検索
		if (skeleton_.joints[i].children.size() != 0) {
			for (int j = 0; j < skeleton_.joints[i].children.size(); j++) {
				int index = skeleton_.joints[i].children[j];
				skeletonLine_[index]->startPos_ = skeleton_.joints[index].transform.translate;
			}
		}
		//skeletonLine_[i]->startPos_ = skeleton_.joints[i].transform.translate;
		//skeletonLine_[i]->endPos_ = skeleton_.joints[i + 1].transform.translate;
	}
}

void Model::JointSphereInit() {
	// メモリーを確保
	jointSphere_.resize(skeleton_.joints.size());

	// 初期化
	for (int i = 0; i < jointSphere_.size(); i++) {
		jointSphere_[i] = new Sphere();
		jointSphere_[i]->Initialize();
	}

	for (int i = 0; i < jointSphere_.size(); i++) {
		jointSphere_[i]->worldTransform.transform.translate = skeleton_.joints[i].transform.translate;
		//jointSphere_[i]->worldTransform.transform.rotate = skeleton_.joints[i].transform.rotate;
		jointSphere_[i]->worldTransform.transform.scale = Multiply(0.1f, skeleton_.joints[i].transform.scale);
	}
}

#pragma region プライベートな関数

void Model::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * modelData_.vertices.size()).Get();
}

void Model::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * UINT(modelData_.vertices.size());
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Model::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = "Engine/resources/" + directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		// 頂点数分のメモリを確保
		modelData.vertices.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			modelData.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };
		}
		// 面からindexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			// 頂点を解析
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}
		// skinClusterの解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			// jointごとの格納領域を作成
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			// InverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale;
			aiVector3D translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix({ scale.x, scale.y, scale.z }, { rotate.x, -rotate.y, -rotate.z, rotate.w }, { -translate.x,translate.y, translate.z });
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			// weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
			}
		}
		if (modelData.skinClusterData.size() == 0) {
			modelData.isSkinClusterData = false;
		}
		else {
			modelData.isSkinClusterData = true;
		}
	}

	// マテリアルの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = "Engine/resources/" + directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	// シーンの階層構造を作成
	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;
}

ModelData Model::LoadModelFile(const std::string& filename) {
	ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = "Engine/resources/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		// 頂点数分のメモリを確保
		modelData.vertices.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			modelData.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };
		}
		// 面からindexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			// 頂点を解析
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}
		// skinClusterの解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			// jointごとの格納領域を作成
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			// InverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale;
			aiVector3D translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix({ scale.x, scale.y, scale.z }, { rotate.x, -rotate.y, -rotate.z, rotate.w }, { -translate.x,translate.y, translate.z });
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			// weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
			}
		}
		if (modelData.skinClusterData.size() == 0) {
			modelData.isSkinClusterData = false;
		}
		else {
			modelData.isSkinClusterData = true;
		}
	}



	// マテリアルの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = "Engine/resources/" + (std::string)textureFilePath.C_Str();
		}
	}

	// シーンの階層構造を作成
	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;
}

#pragma  endregion