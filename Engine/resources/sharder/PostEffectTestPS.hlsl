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


// ガウスブラー
float4 GaussianBlur(float2 uv)
{
	float2 texelSize = 1.0f / float2(1280.0f, 720.0f);

	float blurAmount = sin(50 * 0.02f) * 2;

	float4 blurredColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = -4; i <= 4; ++i) {
		for (int j = -4; j <= 4; ++j)
		{
			float2 offset = texelSize * float2(i, j) * blurAmount;
			float4 sampleColor = gTexture.Sample(gSampler, uv + offset);
			float brightness = dot(sampleColor.rgb, float3(0.299, 0.587, 0.114));
			float b = step(0.3f, brightness);
			blurredColor += (sampleColor * b);
		}
	}

	// カーネルサイズ 9x9なので81で割る
	return blurredColor / 81.0f;
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

	// RGBカラー値をグレースケールに変換
	//output.color = gTexture.Sample(gSampler, input.texcoord);
	//float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
	//output.color.rgb = float32_t3(value, value, value);

	output.color = float32_t4(textureColor.rgb, 1);

	return output;
}