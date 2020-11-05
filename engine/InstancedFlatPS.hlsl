Texture2D ShadowMapNear: register(t6);
Texture2D ShadowMapMid: register(t7);
Texture2D ShadowMapFar: register(t8);
SamplerState ShadowMapSampler: register(s6);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: NORMAL1;
	float3 world_pos: WORLD_POSITION;
};

//world and camera settings
cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
};

//mesh lighting characteristics 
cbuffer constant: register(b1)
{
	float	Ns; //specular power
	float	Ni; //optical density
	float	d;	//transparency 
	float	Tr;   //transparency 
	float	Tf; //transmission filter
	float3	Ka; //ambient color
	float3	Kd; //diffuse color
	float3	Ks; //specular color
	float	Ke; //emissive color
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
	return float4(Kd, d);
}