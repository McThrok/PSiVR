cbuffer perObjectBuffer : register(b0)
{
	float4x4 wvpMatrix;
	float4x4 worldMatrix;
	float4 color;
};

Texture3D b: register(t0);
SamplerState s: register(s0);

struct VS_INPUT
{
	float3 inPos : POSITION;
	float3 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float3 outNormal : NORMAL;
	float3 outWorldPos : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	//float3 pos = b.Sample(s, input.tex);
	//float3 pos = b.SampleLevel(s, 1 - input.tex, 1);
	float3 pos = b.SampleLevel(s, input.tex, 1);
	//pos = input.inPos;

	output.outPosition = mul(wvpMatrix, float4(pos, 1.0f));
	output.outNormal = float3(0, 0, 0);
	output.outWorldPos = mul(worldMatrix, float4(pos, 1.0f));

	return output;
}