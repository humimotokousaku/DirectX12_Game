#include "PostEffectTest.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

const static float kScale = 10.0f;

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

	//// RGBカラー値をグレースケールに変換
	//float brightness = dot(textureColor.rgb, float3(0.299, 0.587, 0.114));

	//if (brightness >= 0.5f) {
	//	output.color = float4(textureColor.rgb, 1);
	//}
	//else {
	//	output.color = float4(0, 0, 0, 1);
	//}

	output.color = float4(textureColor.rgb, 1);
	
	return output;
}