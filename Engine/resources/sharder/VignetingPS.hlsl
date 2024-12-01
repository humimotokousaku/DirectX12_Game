#include "PostEffectTest.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};
struct VignetingData
{
    int32_t isActive; // 使用するか
    float32_t scale;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<VignetingData> gVignetingData : register(b1);

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
    
    if (gVignetingData.isActive)
    {
        output.color = gTexture.Sample(gSampler, input.texcoord);

	    // 周囲を0に、中心になるほど明るくなるように調整
        float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
        float vignette = correct.x * correct.y * gVignetingData.scale;
        vignette = saturate(pow(vignette, 0.8f));
        output.color.rgb *= vignette;
        output.color.a = 1.0f;

        return output;
    }
    
    output.color = float4(textureColor.rgb, 1);
    return output;
}