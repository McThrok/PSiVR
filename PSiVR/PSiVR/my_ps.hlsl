cbuffer perObjectBuffer : register(b0)
{
	float4x4 wvpMatrix;
	float4x4 worldMatrix;
	float4 color;
};

RWTexture2D<float4> uav1;

struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float3 inWorldPos : WORLD_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(0,0,0,1);
	//float3 ambient = float3(0.05,0.05,0.05);
	//float3 diffuse = float3(0.6 ,0.6, 0.6);
	//float3 lightPosition = float3(20, -30, 10);

	//float3 vectorToLight = normalize(lightPosition - input.inWorldPos);
	//float3 diff = max(dot(vectorToLight, input.inNormal), 0);
	//float3 finalColor = color * (ambient + diffuse * diff);

	//return float4(finalColor, 1.0f);
}