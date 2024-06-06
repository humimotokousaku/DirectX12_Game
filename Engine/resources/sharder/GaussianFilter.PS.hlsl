#include "PostEffectTest.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};
struct GaussData {
    int32_t isActive;
    float32_t strength;
};
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<GaussData> gGaussData : register(b1);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 円周率
static const float32_t pi = 3.14f;
static const float32_t2 kIndex[3][3] = {
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

// ガウス
float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * pi * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);
    
    if (gGaussData.isActive)
    {
        float32_t weight = 0.0f;
        float32_t width = 0.0f;
        float32_t height = 0.0f;
        float32_t kernel3x3[3][3];
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

        for (int32_t x = 0; x < 3; ++x)
        {
            for (int32_t y = 0; y < 3; ++y)
            {
			    // 現在のtexcoordを求める
                float32_t2 texcoord = input.texcoord + kIndex[x][y] * uvStepSize;
			    // 色にカーネルの数値をかける
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
			    // ガウスをかける
                kernel3x3[x][y] = gauss(kIndex[x][y].x, kIndex[x][y].y, gGaussData.strength);
			
                output.color.rgb += fetchColor * kernel3x3[x][y];
                weight += kernel3x3[x][y];
            }
        }
        output.color.rgb *= rcp(weight);
        output.color.a = 1.0f;
        return output;
    }

    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = float32_t4(textureColor.rgb, 1.0f);

    return output;
}