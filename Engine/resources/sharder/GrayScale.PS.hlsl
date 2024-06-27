#include "PostEffectTest.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};
struct GrayScaleData
{
    int32_t isActive;
    float32_t strength;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<GrayScaleData> gGrayScaleData : register(b1);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

	// RGBカラー値をグレースケールに変換
    if (gGrayScaleData.isActive)
    {
        output.color = gTexture.Sample(gSampler, input.texcoord);
        float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
        output.color.rgb = float32_t3(value, value, value);
        output.color.a = 1.0f;
        return output;
    }
    
    output.color = float32_t4(textureColor.rgb, 1.0f);
    return output;
}