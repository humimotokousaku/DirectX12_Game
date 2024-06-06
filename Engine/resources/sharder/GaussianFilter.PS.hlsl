#include "PostEffectTest.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
};
struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 円周率
static const float32_t pi = 3.14f;
static const float32_t2 kIndex[3][3] = {
	{{-1.0f,-1.0f}, {0.0f,-1.0f},{1.0f, -1.0f}},
	{{-1.0f,0.0f} , {0.0f,0.0f} ,{1.0f, 0.0f}},
	{{-1.0f,1.0f} , {0.0f,1.0f} ,{1.0f, 1.0f}}
};

// ガウス
float gauss(float x, float y, float sigma) {
	float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
	float denominator = 2.0f * pi * sigma * sigma;
	return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
	float32_t weight = 0.0f;
    float32_t width = 0.0f;
	float32_t height = 0.0f;
	float32_t kernel3x3[3][3];
	float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
	gTexture.GetDimensions(width, height);

	for (int32_t x = 0; x < 3; ++x) {
		for (int32_t y = 0; y < 3; ++y) {
			kernel3x3[x][y] = gauss(kIndex[x][y].x, kIndex[x][y].y,2.0f);			
			weight += kernel3x3[x][y];

		}
	}
    for (int32_t i = 0; i < 3; ++i) {
        for (int32_t j = 0; j < 3; ++j) {
			// 現在のtexcoordを求める
            float32_t2 texcoord = input.texcoord + kIndex[i][j] * uvStepSize;
			// 色にカーネルの数値をかける
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kernel3x3[i][j];
        }
    }
    weight = normalize(weight);
	output.color.rgb *= rcp(weight);
	output.color.a = 1.0f;

	return output;
}