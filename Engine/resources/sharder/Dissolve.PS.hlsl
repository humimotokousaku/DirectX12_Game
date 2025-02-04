#include "PostEffectTest.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};
struct DissolveData
{
    int32_t isActive;
    float32_t maskThreshold;
    float32_t2 edgeRange;
    float32_t3 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DissolveData> gDissolveData : register(b1);
Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    if (gDissolveData.isActive != 0)
    {
        float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
        if (mask <= gDissolveData.maskThreshold)
        {
            discard;
        }
	    // Edgeっぽいものを生成
        float32_t edge = 1.0f - smoothstep(gDissolveData.edgeRange.x, gDissolveData.edgeRange.y, mask);
        output.color = gTexture.Sample(gSampler, input.texcoord);
        output.color.rgb += edge * gDissolveData.color;
        output.color.a = 1.0f;
        return output;
    }
    output.color = gTexture.Sample(gSampler, input.texcoord);
    output.color.a = 1.0f;

    return output;
}