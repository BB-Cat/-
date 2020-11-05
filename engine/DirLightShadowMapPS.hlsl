struct PS_INPUT
{
	float4 position: SV_POSITION;
};

struct PS_OUTPUT
{
	float4 position: SV_POSITION;
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
	PS_OUTPUT output;

	float4 color = input.position.z / input.position.w;
	color.a = 1.0f;

	return color;
}