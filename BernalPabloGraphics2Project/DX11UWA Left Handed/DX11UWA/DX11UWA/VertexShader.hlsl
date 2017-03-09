cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 uv : UV;
	float3 normals : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 WorldPos : W_POSITION;
	float3 uv : UV;
	float3 normals : NORMAL;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	output.WorldPos = pos.xyz;
	
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;
	
	//output.normals = normalize(mul(float4(input.normals, 0.0f),model));
	output.normals = mul(input.normals, (float3x3)model);
	// Pass the color through without modification.
	output.uv = input.uv;

	return output;
}