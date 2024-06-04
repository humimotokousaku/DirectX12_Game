#include "Line.hlsli"

struct ViewProjectionMatrix {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 cameraPos;
};

ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	float32_t4x4 WorldViewProjectionMatrix = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
	output.position = mul(input.position, WorldViewProjectionMatrix);
	return output;
}