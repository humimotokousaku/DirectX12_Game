#include "PostEffectTest.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};
struct RadialBlurData
{
    int32_t isActive; // 使用するか
    float32_t blurWidth;
    float32_t2 center;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<RadialBlurData> gRadialBlurData : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    if (gRadialBlurData.isActive)
    {
        const float32_t2 kCenter = float32_t2(0.5f, 0.5f);
        const int32_t kNumSamples = 10;
        const float32_t kBlurWidth = 0.01f;
        float32_t2 direction = input.texcoord - gRadialBlurData.center;
        float32_t4 outputColor = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int32_t sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
        {
            float32_t2 texcoord = input.texcoord + direction * gRadialBlurData.blurWidth * float32_t(sampleIndex);
            outputColor += gTexture.Sample(gSampler, texcoord);
        }
		// 平均化
        outputColor *= rcp(kNumSamples);
        output.color = outputColor;
        output.color.a = 1.0f;
        return output;
    }

    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = float4(textureColor.rgb, 1);

    return output;
}