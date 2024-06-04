#include "PostEffectTest.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

static const float32_t2 kIndex3x3[3][3] = {
	{{-1.0f,-1.0f}, {0.0f,-1.0f},{1.0f, -1.0f}},
	{{-1.0f,0.0f} , {0.0f,0.0f} ,{1.0f, 0.0f}},
	{{-1.0f,1.0f} , {0.0f,1.0f} ,{1.0f, 1.0f}}
};

static const float32_t kKernel3x3[3][3] = {
	{1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
	{1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
	{1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f}
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	uint32_t width;
	uint32_t height;
	gTexture.GetDimensions(width, height);
	float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
	output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
	output.color.a = 1.0f;
	for (int32_t x = 0; x < 3; ++x) {
		for (int32_t y = 0; y < 3; ++y) {
			// 現在のtexcoordを求める
			float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
			// 色にカーネルの数値をかける
			float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
			output.color.rgb += fetchColor * kKernel3x3[x][y];
		}
	}

	return output;
}