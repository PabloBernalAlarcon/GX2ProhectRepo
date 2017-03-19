// Per-pixel color data passed through the pixel shader.
TextureCube base : register (t0);
SamplerState samples : register(s0);
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 uv : UV;
	
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	return base.Sample(samples,input.uv);
}
