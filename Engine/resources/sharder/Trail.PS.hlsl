#include "Object3d.hlsli"

ConstantBuffer<float32_t3> gColor : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = float32_t4(input.texcoord, 0.0f, 1.0f);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	// textureのα値が0.0以下の時にPixelを棄却
    if (textureColor.a <= 0.0)
    {
        discard;
    }
	
    output.color = gColor * textureColor;
 
    return output;
}
