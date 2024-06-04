#include "PostEffectTest.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t4> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	float32_t4 mask = gMaskTexture.Sample(gSampler, input.texcoord);
	//if (mask <= 0.5f) {
	//	discard;
	//}

	PixelShaderOutput output;
	output.color = gTexture.Sample(gSampler,input.texcoord);
	output.color.a = 1.0f;

	return output;
}