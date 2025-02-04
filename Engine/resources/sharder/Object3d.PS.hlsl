#include "Object3d.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
	float32_t shininess;
};
// 平行光源
struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float intensity;
};
// 点光源
struct PointLight {
	float32_t4 color;
	float32_t3 pos;
	float intensity;
	float radius;
	float decay;
};
// スポットライト
struct SpotLight {
	float32_t4 color;
	float32_t3 pos;
	float intensity;
	float32_t3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
};
// カメラの位置
struct Camera {
	float32_t3 worldPosition;
};
struct DissolveData
{
    int32_t isActive;
    float32_t maskThreshold;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
ConstantBuffer<PointLight> gPointLight : register(b2);
ConstantBuffer<DissolveData> gDissolveData : register(b3);

Texture2D<float32_t4> gTexture : register(t0);
TextureCube<float32_t4> gEnvironmentTexture : register(t1);
Texture2D<float32_t> gMaskTexture : register(t2);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	
	// Dissolve
    if (gDissolveData.isActive)
    {
        float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
        if (mask <= gDissolveData.maskThreshold)
        {
            discard;
        }
    }
	
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	// textureのα値が0.0以下の時にPixelを棄却
	if (textureColor.a <= 0.0) {
		discard;
	}
			
	// 環境マップ
    float32_t3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
    float32_t3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
    float32_t4 enviromentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
		
	// ライティングをする場合
	if (gMaterial.enableLighting != 0) {
		/// 入射光
		// 点光源
        float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.pos);

		/// ハーフランバート
        float NdotL = dot(normalize(input.normal), -pointLightDirection);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		/// 点光源
        float32_t3 pointLightToEye = normalize(gCamera.worldPosition - input.worldPosition);
        float32_t3 pointLightHalfVector = normalize(-pointLightDirection + pointLightToEye);
        float pointLightNdotH = dot(normalize(input.normal), pointLightHalfVector);
        float pointLightSpecularPow = pow(saturate(pointLightNdotH), gMaterial.shininess);
		// 光の減衰
        float32_t distance = length(gPointLight.pos - input.worldPosition);
        float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay);
		
		// 拡散反射
        float32_t3 diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * gPointLight.intensity * factor;
		// 鏡面反射
        float32_t3 specularPointLight = gPointLight.color.rgb * gPointLight.intensity * factor * pointLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);
		// 拡散反射 + 鏡面反射
        output.color.rgb = diffusePointLight;// + specularPointLight;
		// アルファ値
        output.color.a = gMaterial.color.a * textureColor.a;
    }
	else {
        output.color = gMaterial.color * textureColor;// + enviromentColor;
    }

	
	return output;
}
