#include "ModelStructs.h"
#include "SrvManager.h"
#include "Utility.h"
#include <assimp/Importer.hpp>
#include <assert.h>

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;
	std::ifstream file("Engine/resources" + directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスをする
			materialData.textureFilePath = "Engine/resources" + directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}

Motion LoadAnimationFile(const std::string& directoryPath, const std::string& filename) {
	Motion animation;
	Assimp::Importer importer;
	std::string filePath = "Engine/resources/" + directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	// アニメーションを再生しない
	animation.isActive = false;
	// アニメーションがないならリターン
	if (scene->mAnimations == 0) {
		return animation;
	}
	// アニメーションの再生速度
	animation.playBackSpeed = 1.0f;
	// ループ再生
	animation.isLoop = false;

	aiAnimation* animationAssimp = scene->mAnimations[0];
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変換

	// それぞれのNodeAnimationの情報を取得
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		// Translateの解析
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 左手座標から右手座標に変換
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		// Rotateの解析
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 左手座標から右手座標に変換
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		// Scaleの解析
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}

	return animation;
}
Motion LoadAnimationFile(const std::string& filename) {
	Motion animation;
	Assimp::Importer importer;
	std::string filePath = "Engine/resources/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	// アニメーションを再生しない
	animation.isActive = false;
	// アニメーションがないならリターン
	if (scene->mAnimations == 0) {
		return animation;
	}
	// アニメーションの再生速度
	animation.playBackSpeed = 1.0f;
	// ループ再生
	animation.isLoop = false;

	aiAnimation* animationAssimp = scene->mAnimations[0];
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変換

	// それぞれのNodeAnimationの情報を取得
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		// Translateの解析
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 左手座標から右手座標に変換
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		// Rotateの解析
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 左手座標から右手座標に変換
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		// Scaleの解析
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}

	return animation;
}

Node ReadNode(aiNode* node) {
	Node result;
	aiVector3D scale;
	aiVector3D translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);
	result.transform.scale = { scale.x, scale.y, scale.z };
	result.transform.rotate = { rotate.x, -rotate.y, -rotate.z,rotate.w };
	result.transform.translate = { -translate.x, translate.y, translate.z };
	result.localMatrix = MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);

	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読む
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());
	joint.parent = parent;
	joints.push_back(joint);
	for (const Node& child : node.children) {
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	return joint.index;
}

Skeleton CreateSkeleton(const Node& rootNode) {
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	// 名前とindexのマッピング
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	SkeletonUpdate(skeleton);

	return skeleton;
}
void SkeletonUpdate(Skeleton& skeleton) {
	for (Joint& joint : skeleton.joints) {
		joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
		if (joint.parent) {
			joint.skeletonSpaceMatrix = Multiply(joint.localMatrix, skeleton.joints[*joint.parent].skeletonSpaceMatrix);
		}
		else {
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData) {
	SkinCluster skinCluster;
	// クラスターデータがないなら早期リターン
	if (!modelData.isSkinClusterData) {
		return skinCluster;
	}

#pragma region palette用のResourceを確保
	skinCluster.paletteResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette, skeleton.joints.size() };
#pragma  endregion

#pragma region pallet用のsrv作成
	skinCluster.srvIndex = SrvManager::GetInstance()->Allocate();
	skinCluster.paletteSrvHandle.first = SrvManager::GetInstance()->GetCPUDescriptorHandle(skinCluster.srvIndex);
	skinCluster.paletteSrvHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(skinCluster.srvIndex);
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(skinCluster.srvIndex, skinCluster.paletteResource.Get(), (UINT)skeleton.joints.size(), sizeof(WellForGPU));
#pragma endregion

#pragma region Influence用のResourceを確保
	skinCluster.influenceResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mappedInfluence = { mappedInfluence, modelData.vertices.size() };

	// VBVを作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	// InverseBindPoseMatrixのサイズを確保
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4);
#pragma endregion

#pragma region ModelのSkinClusterを解析
	for (const auto& jointWeight : modelData.skinClusterData) {
		auto it = skeleton.jointMap.find(jointWeight.first);
		// jointの名前を検索
		if (it == skeleton.jointMap.end()) {
			continue;
		}

		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}
#pragma endregion

	SkinClusterUpdate(skinCluster, skeleton);

	return skinCluster;
}

void SkinClusterUpdate(SkinCluster& skinCluster, const Skeleton& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix = Multiply(skinCluster.inverseBindPoseMatrices[jointIndex], skeleton.joints[jointIndex].skeletonSpaceMatrix);
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Transpose(Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}

Vector3 CalculateTranslateValue(const std::vector<KeyframeVector3>& keyframes, float time) {
	assert(!keyframes.empty());
	// キーが一つか、時刻がキーフレーム前なら最初の値にする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexを比較して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerps::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}
Quaternion CalculateQuaternionValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
	assert(!keyframes.empty());
	// キーが一つか、時刻がキーフレーム前なら最初の値にする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexを比較して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return keyframes.rbegin()->value;
}
Vector3 CalculateScaleValue(const std::vector<KeyframeVector3>& keyframes, float time) {
	assert(!keyframes.empty());
	// キーが一つか、時刻がキーフレーム前なら最初の値にする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexを比較して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerps::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
	HRESULT hr;
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; // リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれからは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// 実際に頂点リソースを作る
	hr = device.Get()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}