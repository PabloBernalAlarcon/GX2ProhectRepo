texture2D base : register(t0);
SamplerState samp : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 WorldPos : W_POSITION;
	float3 uv : UV;
	float3 normals : NORMAL;
};

float4 DL(PixelShaderInput input)
{
	float3 LightDirection = { 1.0f, 0.0f, 0.0f };
	float LightRatio = clamp(dot(-normalize(LightDirection), input.normals), 0, 1);
	float3 Color = { 1.0f, 1.0f, 1.0f };
	float4 Return = float4(Color, 1.0f) * LightRatio;
	return Return;
}

float4 PL(PixelShaderInput input) {
	float3 lightpos;
	lightpos.x = -1.0f - input.WorldPos.x;
	lightpos.y = 1.0f - input.WorldPos.y;
	lightpos.z = 1.0f - input.WorldPos.z;
	float3 lightdir= normalize(lightpos);
	float lightratio = clamp(dot(normalize(lightdir), input.normals), 0.0f, 1.0f);
	float3 Color = { 1.0f, 0.0f, 0.0f };
	float4 result = float4(Color, 1.0f)*lightratio;
	return result;
}

float4 SL(PixelShaderInput input)
{
	float3 lightpos;
	lightpos.x = 2.0f - input.WorldPos.x;
	lightpos.y = 3.0f - input.WorldPos.y;
	lightpos.z = 1.0f - input.WorldPos.z;
	//Set cone Direction
	float3 Cone = { 0.0f, -1.0f, 0.0f };
	//Set Surface Normals
	float3 SurfaceNormals = input.normals;
	//Set Light Direction to Normalized light pos
	float3 lightDir = normalize(lightpos);
	//Calculate the Surface Ratio
	float3 SurfaceRatio = saturate(dot(-normalize(lightDir), normalize(Cone)));
	//Set your spot light cone 
	float spotfactor = (SurfaceRatio > 0.7f) ? 1 : 0;
	float lightratio = saturate(dot(normalize(lightDir), input.normals));
	
	//Calculate Result
	float3 Color = { 0.0f, 0.0f, 1.0f };
	float4 result = float4(Color, 1.0f)*lightratio*spotfactor;
	//Return
	return result;
}
// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 DirectionalLight = DL(input);
	float4 PointLight = PL(input);
	float4 SpotLight = SL(input);

	
	//return base.Sample(samp, input.uv) * DirectionalLight;
	//return base.Sample(samp, input.uv) * PointLight;
	//return base.Sample(samp, input.uv) * SpotLight;
	float4 All = DirectionalLight + PointLight + SpotLight;
	return base.Sample(samp, input.uv) * All;
}
