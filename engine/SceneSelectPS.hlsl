Texture2D Texture: register(t0);
SamplerState TextureSampler: register(s0);

cbuffer constant: register(b7)
{
	float m_time; //millisecond timer
	float m_elapsed; //time since the last frame
};

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
};

float weight(float t, float log2radius, float gamma)
{
	return exp(-gamma * pow(log2radius - t, 2.0));
}



float4 psmain(PS_INPUT input) : SV_TARGET
{
	float2 coord = input.texcoord * 2.0 - 1.0;
	coord.x *= 1.388;
	coord = (coord) + float2(0.15 * cos(m_time * 0.2), 0.15 * sin(m_time * 0.2));

	// get polar coordinates    
	float a = atan2(coord.y, coord.x);
	float b = atan2(coord.y, abs(coord.x));
	float r = length(coord);

	// pack and animate    
	float2 uvR = float2(1.0 / r + m_time * 0.01, b / 3.1415927);
	//uvR = float2(sin(uvR.x), -cos(uvR.y)) * m_time / 30;
	float2 uv = float2(atan2(coord.y, coord.x), 0.4 / (r)) + m_time / 30.0;

	float3 col1 = Texture.SampleLevel(TextureSampler, uvR, 6 * (1.0 - length(coord))).rgb;
	float3 col2 = 0;//Texture.SampleLevel(TextureSampler, uv, length(uv) * 5).rgb;


	float3 pix = 0;
	float norm = 0.0;
	float gamma = 0.5;
	float radius = 20 * max(1.0 / 20.0, (pow(length(coord), 10)));

	for (float i = 0.0; i < 10.0; i += 0.5)
	{
		float k = weight(i, log2(radius), gamma);
		pix += k * Texture.SampleLevel(TextureSampler, uv, i).rgb;
		norm += k;
	}

	col2 = pix * pow(norm, -0.95);

	float3 color = (col1 + (col2 * 0.3));
	color = color * r + float3(0.95, 0.99, 0.99) * (1 - r);
	return float4(color, 1);
}