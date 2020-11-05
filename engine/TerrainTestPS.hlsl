struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 normal: NORMAL0;
	float4 world_pos : TEXCOORD0;
	float4 texcolor : WEIGHTS;
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float4 color = (0, 0, 1, 1);
	//color.r = 1 * ((int)(input.world_pos.x) % 2 == 1);
	//color.g = 1 * ((int)(input.world_pos.z) % 2 == 1);

	int r = input.world_pos.x;
	int g = input.world_pos.z;

	color.r = 1 * (r % 2 == 1);
	color.g = 1 * (g % 2 == 1);

	//return color;
	//return float4(input.normal.xyz, 1);

	float tex = 1.0 + input.texcolor.w;
	return float4(tex, tex, tex, tex);
	//return input.texcolor;

	//r = input.texcolor.x / input.texcolor.xyz;
	//g = input.texcolor.y / input.texcolor.xyz;
	//float b = input.texcolor.z / input.texcolor.xyz;
	//return float4(r, g, b, 1);
}
