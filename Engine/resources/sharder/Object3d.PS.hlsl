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

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);
ConstantBuffer<Camera> gCamera : register(b2);

Texture2D<float32_t4> gTexture : register(t0);
TextureCube<float32_t4> gEnvironmentTexture : register(t1);
SamplerState gSampler : register(s0);
struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

float32_t3 RGBtoHSV(float32_t3 rgb)
{

        float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
        float4 p = rgb.g < rgb.b ? float4(rgb.bg, K.wz) : float4(rgb.gb, K.xy);
        float4 q = rgb.r < p.x ? float4(p.xyw, rgb.r) : float4(rgb.r, p.yzx);

        float d = q.x - min(q.w, q.y);
        float e = 1.0e-10;
        return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
float3 HSVtoRGB(float3 hsv)
{
    float H = hsv.x;
    float S = hsv.y;
    float V = hsv.z;

    float C = V * S;
    float X = C * (1.0 - abs(fmod(H * 6.0, 2.0) - 1.0));
    float m = V - C;

    float3 rgb;

    if (H < 1.0 / 6.0)
    {
        rgb = float3(C, X, 0.0);
    }
    else if (H < 2.0 / 6.0)
    {
        rgb = float3(X, C, 0.0);
    }
    else if (H < 3.0 / 6.0)
    {
        rgb = float3(0.0, C, X);
    }
    else if (H < 4.0 / 6.0)
    {
        rgb = float3(0.0, X, C);
    }
    else if (H < 5.0 / 6.0)
    {
        rgb = float3(X, 0.0, C);
    }
    else
    {
        rgb = float3(C, 0.0, X);
    }

    return rgb + float3(m, m, m);
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	// HSVに変換
    float32_t3 hsv = RGBtoHSV(textureColor.rgb);
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

		///// ハーフランバート
		//float NdotL = dot(normalize(input.normal), -pointLightDirection);
		//float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		/// 点光源
		float32_t3 pointLightToEye = normalize(gCamera.worldPosition - input.worldPosition);
		float32_t3 pointLightHalfVector = normalize(-pointLightDirection + pointLightToEye);
		float pointLightNdotH = dot(normalize(input.normal), pointLightHalfVector);
		float pointLightSpecularPow = pow(saturate(pointLightNdotH), gMaterial.shininess);
		// 光の減衰
		float32_t distance = length(gPointLight.pos - input.worldPosition);
		float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay);
		// 拡散反射
        float32_t3 diffusePointLight = gMaterial.color.rgb * HSVtoRGB(hsv) * gPointLight.color.rgb * gPointLight.intensity * factor;
		// 鏡面反射
		float32_t3 specularPointLight = gPointLight.color.rgb * gPointLight.intensity * factor * pointLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);
		
		// 拡散反射 + 鏡面反射
		output.color.rgb = diffusePointLight + specularPointLight;
		// アルファ値
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	else {
        output.color = gMaterial.color * float32_t4(HSVtoRGB(hsv), textureColor.a) + enviromentColor;
    }

	return output;
}
