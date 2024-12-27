#pragma once
#include "MathStructs.h"
#include <map>
#include <optional>
#include <vector>
#include <array>
#include <span>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>
#include <assimp/scene.h>
#include <d3d12.h>
#include <wrl.h>

struct TransformationMatrix {
	Matrix4x4 matWorld;
	Matrix4x4 WorldInverseTrasnpose;
};
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
};
struct MaterialData {
	std::string textureFilePath;
};

#pragma region ノード
template<typename tValue>
struct Keyframe {
	float time;
	tValue value;
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

template<typename tValue>
// キーフレーム
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};
// ノードアニメーション
struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};
// アニメーション
struct Motion {
	float duration;	// アニメーション全体の尺(秒)
	float time;		// アニメーションの経過時間
	float playBackSpeed; // アニメーションの再生速度
	bool isActive;	// アニメーションの再生
	bool isLoop;    // ループ再生
	const char* name;     // デバッグ用に名前を付ける
	std::map<std::string, NodeAnimation> nodeAnimations;
	// アニメーションの再生開始
	void Play() { isActive = true; }
	// アニメーションを止める
	void Stop() { isActive = false; }
};

// ノード
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

#pragma endregion

#pragma region SkeletonとJoint
struct Joint {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix;
	std::string name;
	std::vector<int32_t> children;
	int32_t index;
	std::optional<int32_t> parent;
};

struct Skeleton {
	int32_t root;
	std::map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
};
#pragma endregion

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix;
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};
struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};
struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
	uint32_t srvIndex;
};
struct ModelData {
	std::map<std::string, JointWeightData> skinClusterData;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
	Node rootNode;
	bool isSkinClusterData;
};

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

// アニメーションの読み込み
Motion LoadAnimationFile(const std::string& directoryPath, const std::string& filename);
Motion LoadAnimationFile(const std::string& filename);

// ノードの読み込み
Node ReadNode(aiNode* node);

// Jointを作成
int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

// スケルトンを作成
Skeleton CreateSkeleton(const Node& rootNode);
// スケルトンの更新処理
void SkeletonUpdate(Skeleton& skeleton);

// skinClusterの作成
SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData);
// skinClusterの更新
void SkinClusterUpdate(SkinCluster& skinCluster, const Skeleton& skeleton);

Vector3 CalculateTranslateValue(const std::vector<KeyframeVector3>& keyframes, float time);
Quaternion CalculateQuaternionValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
Vector3 CalculateScaleValue(const std::vector<KeyframeVector3>& keyframes, float time);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);