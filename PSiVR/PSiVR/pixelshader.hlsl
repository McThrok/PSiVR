struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 pixelColor = float3(0.5, 0.75, 0.5);
	return float4(pixelColor, 1.0f);
}