Texture2D Texture: register(t0);
SamplerState TextureSampler: register(s0);

#include "hex.fx"

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

//float4 psmain(PS_INPUT input) : SV_TARGET
//{
//	float divisor = 60;
//	float sinval = (sin((input.position.x + m_time * 2 * divisor) / divisor) + 1) / 2;
//	float sinval2 = (sin((input.position.x + m_time * 5.2 * divisor * 1.2) / divisor * 1.2) + 1) / 2;
//	float sinval3 = (sin((input.position.x + m_time * 1.0 * divisor * 0.4) / divisor * 1.4) + 1) / 2;
//
//	float sinval4 = (sin((input.position.x + m_time * 1.0 * divisor * 3) / divisor * 3) + 1) / 2;
//	
//	float heightmod = (sin(frac(m_time * 2 / (3.1415 * 3)) * 3.1415 * 2.0) + 1) / 10;
//	float heightmod2 = (sin(frac((m_time + 0.4) * 2 / (3.1415 * 3)) * 3.1415 * 2) + 1) / 40;
//	float heightmod3 = (sin(frac((m_time + 0.2) * 2 / (3.1415 * 3)) * 3.1415 * 2) + 1) / 40;
//
//	
//	heightmod = heightmod * 0.3;
//	float threshold =  floor((sinval  * heightmod + (1.0 - heightmod) / 2  ) * 150.0) / 150.0;
//	float threshold2 = floor((sinval2 * heightmod2 + (1.0 - heightmod2) / 2) * 150.0) / 150.0;
//	float threshold3 = floor((sinval3 * heightmod3 + (1.0 - heightmod3) / 2) * 150.0) / 150.0;
//
//	float threshold4 = floor((sinval4 * heightmod + (1.0 - heightmod) / 2) * 150.0) / 150.0;
//
//	float3 color = float3(0.2, 0.25, 0.3);  
//	float3 color2 = float3(0.7, 0.8, 0.5);
//	if(input.texcoord.y >= threshold) color.r = color2.r;
//	if(input.texcoord.y >= threshold2) color.g = color2.g;
//	if(input.texcoord.y >= threshold3) color.b = color2.b;
//
//	bool xchecker = (int)(input.texcoord.x * 1.33 * 50) % 2;
//	bool ychecker = (input.texcoord.y < threshold4 - 0.2 && input.texcoord.y > threshold4 - 0.22);
//	if (ychecker && xchecker) color = color2;
//
//	return float4(color, 1);
//}



//float4 psmain(PS_INPUT input) : SV_TARGET
//{
//	float2 coord = input.texcoord * 2.0 - 1.0;
//	coord.x *= 1.388;
//	coord = (coord) + float2(0.15 * cos(m_time * 0.2), 0.15 * sin(m_time * 0.2));
//
//	// get polar coordinates    
//	float a = atan2(coord.y, coord.x);
//	float b = atan2(coord.y, abs(coord.x));
//	float r = length(coord);
//
//	// pack and animate    
//	//float2 uvL = float2(1.0 / r + m_time * 0.2, a / 3.1415927);
//	float2 uvR = float2(1.0 / r + m_time * 0.05, b / 3.1415927);
//
//
//
//	//float a = atan2(coord.y, coord.x) * 1.1;
//	float2 uv = float2(atan2(coord.y, coord.x), 0.4 / (r)) + m_time / 30.0;
//
//	float3 col1 = Texture.Sample(TextureSampler, uvR).rgb;
//	float3 col2 = Texture.Sample(TextureSampler, uv).rgb;
//	float3 color = (col1 + (col2 * 0.3));
//	color = color * r + float3(0.95, 0.99, 0.99) * (1 - r);
//	return float4(color, 1);
//}



float4 psmain(PS_INPUT input) : SV_TARGET
{
    float2 coord = input.texcoord * 2.0 - 1.0;
    coord.x *= 1.388;
    float3 color = 0;

	//float2 uv = abs(coord);

	coord *= 5.0;

	float2 normal, normal2;
	float4 hc = hexCoords(coord, normal);
	float4 hc2 = hexCoords(coord * 2, normal2);
	normal = (normal * 2 + normal2) / 3;

	float c = smoothstep(0.01, 0.02, hc.y * sin(((hc.z + 10) * (hc.w + 20)) + (m_time / 3)));
	//c = step(0.3, c);

	float3 dark = float3(0.1, 0.14, 0.2);
	float3 light = float3(0.2, 0.4, 0.4);
	color += light * c + dark * (1 - c);

	float3 lightpos = float3(9.25 * cos(m_time * 0.8), 7.25 * sin(m_time * 1.3), 1.5);
	float3 lightvec = lightpos - float3(coord.xy, 0);
	float len = length(lightvec);
	float light_amount = 1.0 - min(1, len / 15.5);
	light_amount = pow(light_amount, 3);

	if (c)
	{
		float3 normal3d = normalize(float3(normal.xy, 0.3));
		//if (hc.y > 0.075) normal3d = float3(0, 0, 1);
		if ((hc.y > 0.095 && hc.y < 0.105) || hc2.y > 0.075) normal3d =  normal3d * 0.5;
		if (hc.y > 0.16) normal3d = float3(0, 0, 1);
		else if (hc.y > 0.075)normal3d = float3(-normal3d.xy, normal3d.z);

		float dot_light = max(dot(lightvec / len, normal3d), 0);
		color += float3(0.9, 0.85, 0.3) * dot_light;
		color = saturate(color);
	}
	else color += float3(0.2, 0.15, 0.05) * light_amount;


	color += float3(0.15, 0.1, 0.025) * (0.5 + -coord.y * 0.35);


    return float4(color, 1);

}