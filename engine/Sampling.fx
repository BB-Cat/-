float3 getTextureSplat(float3 tex1, float3 tex2, float3 tex3, float3 bias)
{
	float3 output = 0;
	
	output += tex1 * (bias.r >  bias.g && bias.r >  bias.b);
	output += tex2 * (bias.g >  bias.r && bias.g >  bias.b);
	output += tex3 * (bias.b >= bias.r && bias.b >= bias.g);

	return output;
}