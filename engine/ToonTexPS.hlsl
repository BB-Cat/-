#include "Lighting.fx"
#include "General.fx"

Texture2D Texture: register(t0);
SamplerState TextureSampler: register(s0);

Texture2D NormalTexture: register(t1);

Texture2D GlossTexture: register(t2);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 binormal : NORMAL1;
	float3 tangent : NORMAL2;
	float3 direction_to_camera : NORMAL3;
	float4 lightcolor: TEXCOORD2;
};

//mesh lighting characteristics 
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

//scene global light settings
cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	//normal calculation
	float3x3 inverseMatrix = buildInverseMatrix(input.normal, input.binormal, input.tangent);
	float3 normal = NormalTexture.Sample(TextureSampler, input.texcoord).xyz + float3(0, 0, 0.5f);
	normal = normalize(mul(normal, inverseMatrix));


	float atten = m_global_light_strength;
	float3 light_dir = normalize(m_global_light_dir.xyz);

	//diffuse
	//float3 diffuse_reflection = diffuse(normal, light_dir, m_global_light_color);
	float3 diffuse_reflection = gradientOneCellDiffuse(normal, light_dir, m_global_light_color.rgb, (1.0 - atten));

	//ambient
	float3 ambient_dir = normalize(m_global_light_dir.xyz) * -1;
	float3 ambient_reflection = 0.5 * m_ambient_light_color.rgb * max(0.0, dot(normal, ambient_dir)) * atten;


	float3 lightFinal = (diffuse_reflection + ambient_reflection) + input.lightcolor;

	float3 sample_color = Texture.Sample(TextureSampler, input.texcoord);

	float3 final = cellStroke(input.normal, -input.direction_to_camera, sample_color * lightFinal, float3(0.3, 0.1, 0.1), 0.75);

	return float4(final, m_d);

}