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

float3 GetB(float t, float3 p0, float3 p1, float3 p2, float3 p3)
{
	float c = 1 - t;
	return p0 * c * c * c + 3 * p1 * c * c * t + 3 * p2 * c * t * t + p3 * t * t * t;
}

float3 GetPosition(float3 uvw)
{
	float3 x00 = GetB(uvw.x, b.SampleLevel(s, float3(0, 0.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 0.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 0.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(1, 0.0f / 3, 0.0f / 3), 1));
	float3 x10 = GetB(uvw.x, b.SampleLevel(s, float3(0, 1.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 1.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 1.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(1, 1.0f / 3, 0.0f / 3), 1));
	float3 x20 = GetB(uvw.x, b.SampleLevel(s, float3(0, 2.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 2.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 2.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(1, 2.0f / 3, 0.0f / 3), 1));
	float3 x30 = GetB(uvw.x, b.SampleLevel(s, float3(0, 3.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 3.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 3.0f / 3, 0.0f / 3), 1), b.SampleLevel(s, float3(1, 3.0f / 3, 0.0f / 3), 1));
	float3 x01 = GetB(uvw.x, b.SampleLevel(s, float3(0, 0.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 0.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 0.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(1, 0.0f / 3, 1.0f / 3), 1));
	float3 x11 = GetB(uvw.x, b.SampleLevel(s, float3(0, 1.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 1.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 1.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(1, 1.0f / 3, 1.0f / 3), 1));
	float3 x21 = GetB(uvw.x, b.SampleLevel(s, float3(0, 2.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 2.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 2.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(1, 2.0f / 3, 1.0f / 3), 1));
	float3 x31 = GetB(uvw.x, b.SampleLevel(s, float3(0, 3.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 3.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 3.0f / 3, 1.0f / 3), 1), b.SampleLevel(s, float3(1, 3.0f / 3, 1.0f / 3), 1));
	float3 x02 = GetB(uvw.x, b.SampleLevel(s, float3(0, 0.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 0.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 0.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(1, 0.0f / 3, 2.0f / 3), 1));
	float3 x12 = GetB(uvw.x, b.SampleLevel(s, float3(0, 1.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 1.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 1.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(1, 1.0f / 3, 2.0f / 3), 1));
	float3 x22 = GetB(uvw.x, b.SampleLevel(s, float3(0, 2.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 2.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 2.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(1, 2.0f / 3, 2.0f / 3), 1));
	float3 x32 = GetB(uvw.x, b.SampleLevel(s, float3(0, 3.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 3.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 3.0f / 3, 2.0f / 3), 1), b.SampleLevel(s, float3(1, 3.0f / 3, 2.0f / 3), 1));
	float3 x03 = GetB(uvw.x, b.SampleLevel(s, float3(0, 0.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 0.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 0.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(1, 0.0f / 3, 3.0f / 3), 1));
	float3 x13 = GetB(uvw.x, b.SampleLevel(s, float3(0, 1.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 1.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 1.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(1, 1.0f / 3, 3.0f / 3), 1));
	float3 x23 = GetB(uvw.x, b.SampleLevel(s, float3(0, 2.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 2.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 2.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(1, 2.0f / 3, 3.0f / 3), 1));
	float3 x33 = GetB(uvw.x, b.SampleLevel(s, float3(0, 3.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(1.0f / 3, 3.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(2.0f / 3, 3.0f / 3, 3.0f / 3), 1), b.SampleLevel(s, float3(1, 3.0f / 3, 3.0f / 3), 1));

	float3 xy0 = GetB(uvw.y, x00, x10, x20, x30);
	float3 xy1 = GetB(uvw.y, x01, x11, x21, x31);
	float3 xy2 = GetB(uvw.y, x02, x12, x22, x32);
	float3 xy3 = GetB(uvw.y, x03, x13, x23, x33);

	float3 xyz = GetB(uvw.z, xy0, xy1, xy2, xy3);

	return xyz;
}

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float3 pos = GetPosition(input.tex);

	output.outPosition = mul(wvpMatrix, float4(pos, 1.0f));
	output.outNormal = float3(0, 0, 0);
	output.outWorldPos = mul(worldMatrix, float4(pos, 1.0f));

	return output;
}

