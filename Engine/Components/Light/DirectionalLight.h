#pragma once
#include "MathStructs.h"
#include <d3d12.h>
#include <wrl.h>

struct DirectionalLightGPU {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

class DirectionalLight
{
public:
	static DirectionalLight* GetInstance();

	Microsoft::WRL::ComPtr<ID3D12Resource> GetDirectionalLightResource() { return directionalLightResource_.Get(); }
	void SetDirectionalLightResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& directionalLightResource) { directionalLightResource_ = directionalLightResource.Get(); }

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	void CreateDirectionalResource();

	~DirectionalLight();

	void Initialize();

	void ImGuiAdjustParameter();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLightGPU* directionalLightData_;
};