Texture2D Texture: register(t0);
SamplerState TextureSampler: register(s0);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
};

cbuffer constant: register(b1)
{
	float		m_shininess; //specular power
	float		m_rimPower; //rimlight power
	float		m_d;	//transparency 
	float		m_metallicAmount; //amount of metallicism
	float4		m_ambientColor; //ambient color
	float4		m_diffuseColor; //diffuse color
	float4		m_specularColor; //specular color
	float4		m_emitColor; //emissive color
	float4		m_rimColor; //rimlight color
};

float4 psmain(PS_INPUT input) : SV_TARGET
{

	//float3 tex = Texture.Sample(TextureSampler, input.texcoord).rgb;
	//float4 color = float4(tex.xyz, 1);
	//return color;
	float4 col = Texture.Sample(TextureSampler, input.texcoord);
	col.w *= m_d;
	return  col;
}