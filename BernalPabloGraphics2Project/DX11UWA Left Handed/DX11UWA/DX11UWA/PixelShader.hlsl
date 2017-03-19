texture2D base : register(t0);
SamplerState samp : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 WorldPos : W_POSITION;
	float3 uv : UV;
	float3 normals : NORMAL;
	float3 lightval : COLOR;
};

float4 DL(PixelShaderInput input)
{
	float3x3 lightmovementz = { cos(input.lightval.x),-sin(input.lightval.x),0.0f,
								sin(input.lightval.x), cos(input.lightval.x),0.0f,
								0.0f,0.0f,1.0f };
	
	float3 LightDirection = { 0.0f, 0.3f, 1.0f };
	LightDirection.x -= input.WorldPos.x;
	LightDirection.y -= input.WorldPos.y;
	LightDirection.z -= input.WorldPos.z;
	LightDirection = mul(LightDirection, lightmovementz);
	float LightRatio = saturate(dot(-normalize(LightDirection), input.normals));
	float3 Color = { 1.0f, 1.0f, 1.0f };
	float4 Return = float4(Color, 1.0f) * LightRatio;
	return Return;
}

float4 PL(PixelShaderInput input) {
	float3 lightpos;
	float3x3 lightmovementy = { cos(input.lightval.x),0.0f,sin(input.lightval.x),
												0.0f,1.0f,0.0f,
							-sin(input.lightval.x),0.0f, cos(input.lightval.x) };
	float3 wposition = input.WorldPos.xyz;

	wposition = mul(wposition, lightmovementy);
	
	lightpos.x = -3.0f - wposition.x;
	lightpos.y = 1.0f - wposition.y;
	lightpos.z = 1.0f - wposition.z;
	float mag = sqrt(lightpos.x*lightpos.x + lightpos.y*lightpos.y + lightpos.z*lightpos.z);
	float atenuation = 1.0f - saturate(mag / 10.0f);
	float3 lightdir= normalize(lightpos);
	float lightratio = saturate(dot(normalize(lightdir), input.normals));
	float3 Color = { 1.0f, 0.0f, 0.0f };
	float4 result = float4(Color, 1.0f)*lightratio*atenuation;
	return result;

}

float4 SL(PixelShaderInput input)
{
	float3 lightpos;
	float3x3 lightmovementy = { cos(input.lightval.x),0.0f,sin(input.lightval.x),
		0.0f,1.0f,0.0f,
		-sin(input.lightval.x),0.0f, cos(input.lightval.x) };
	float3 wposition = input.WorldPos.xyz;

	wposition = mul(wposition, lightmovementy);

	lightpos.x = 2.0f - wposition.x;
	lightpos.y = 3.0f - wposition.y;
	lightpos.z = 1.0f - wposition.z;
	//Set cone Direction
	float3 Cone = { 0.0f, -1.0f, 0.0f };
	float3x3 lightmovementz = { cos(input.lightval.x),-sin(input.lightval.x),0.0f,
		sin(input.lightval.x), cos(input.lightval.x),0.0f,
		0.0f,0.0f,1.0f };

	Cone = mul(Cone, lightmovementz);
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

	float4 ModelColor = base.Sample(samp, input.uv);
	//return base.Sample(samp, input.uv) * DirectionalLight;
	//return base.Sample(samp, input.uv) * PointLight;
	//return base.Sample(samp, input.uv) * SpotLight;
	float4 All = DirectionalLight + PointLight +SpotLight;
	return base.Sample(samp, input.uv) * All*ModelColor;
}
