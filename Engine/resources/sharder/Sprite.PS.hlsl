#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};
struct DissolveData
{
    int32_t isActive;
    float32_t maskThreshold;
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

	// Dissolve
    if (gDissolveData.isActive != 0)
    {
        float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
        if (mask <= gDissolveData.maskThreshold)
        {
            discard;
        }
    }
	
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	// textureのα値が0.0以下の時にPixelを棄却
    if (textureColor.a <= 0.0)
    {
        discard;
    }
	
    output.color = gMaterial.color * textureColor;

    return output;
}
