cbuffer perObjectBuffer : register(b0)
{
	float4x4 wvpMatrix;
	float4x4 worldMatrix;
	float4 color;
};

struct VS_INPUT
{
	float3 inPos : POSITION;
};

struct VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float3 outWorldPos : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float3 pos = input.inPos;

	output.outPosition = mul(wvpMatrix, float4(pos, 1.0f));
	output.outWorldPos = mul(worldMatrix, float4(pos, 1.0f));

	return output;
}