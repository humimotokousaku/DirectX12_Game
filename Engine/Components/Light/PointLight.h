#pragma once
#include "MathStructs.h"
#include <d3d12.h>
#include <wrl.h>

struct PointLightforGPU {
	Vector4 color;
	Vector3 pos;
	float intensity;
	float radius;
	float decay;
	float padding[2];
};

class PointLight
{
public:
	static PointLight* GetInstance();

	PointLight() = default;
	~PointLight();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 全ての情報をリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// ImGuiで調整する各パラメータ
	/// </summary>
	void ImGuiParameter();

	Microsoft::WRL::ComPtr<ID3D12Resource> GetPointLightResource() { return pointLightResource_.Get(); }
	void SetPointLightResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& pointLightResource) { pointLightResource_ = pointLightResource.Get(); }
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	void CreatePointLightResource();

#pragma region Setter
	/// <summary>
	/// ライトの座標を設定
	/// </summary>
	/// <param name="pos">座標</param>
	void SetPosition(const Vector3& pos) { pointLightData_->pos = pos; }
	/// <summary>
	/// ライトの色を設定
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color) { pointLightData_->color = color; }
	/// <summary>
	/// ライトの明るさを設定
	/// </summary>
	/// <param name="intensity">明るさ</param>
	void SetIntensity(const float& intensity) { pointLightData_->intensity = intensity; }
	/// <summary>
	/// ライトの減衰率を設定
	/// </summary>
	/// <param name="decay">減衰率</param>
	void SetDecay(const float& decay) { pointLightData_->decay = decay; }
	/// <summary>
	/// ライトの半径
	/// </summary>
	/// <param name="radius">半径</param>
	void SetRadius(const float& radius) { pointLightData_->radius = radius; }

	/// <summary>
	/// 追従対象の座標を設定
	/// </summary>
	/// <param name="pos">追従対象の座標</param>
	void SetTargetPosition(Vector3* pos) { targetPos_ = pos; }
	/// <summary>
	/// 追従対象との距離を設定
	/// </summary>
	/// <param name="offset">追従対象との距離</param>
	void SetTargetOffset(Vector3 offset) { targetOffset_ = offset; }
#pragma endregion

private:
	// 追従対象の座標
	Vector3* targetPos_;
	// 追従対象との距離
	Vector3 targetOffset_;

	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	PointLightforGPU* pointLightData_;
};