#include "PostEffectTest.hlsli"

// 構造体
struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};
struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

// レジスタ
ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 静的変数
static const float32_t kPrewittHorizontalKernel[3][3] = {
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};
static const float32_t kPrewittVerticalKernel[3][3] = {
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

// 関数
float32_t Luminance(float32_t3 v) {
    return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input) {
    float32_t2 difference = float32_t2(0.0f, 0.0f);
    
    for (int32_t x = 0; x < 3; ++x) 
    {
        
    }
    
        PixelShaderOutput output;
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    output.color = float32_t4(textureColor.rgb, 1);

    return output;
}