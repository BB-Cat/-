
//world and camera settings -- UNUSED RIGHT NOW!!
cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
};

//mesh lighting characteristics 
cbuffer constant: register(b1)
{
	float		m_shininess; //specular power
	float		m_rimPower; //rimlight power
	float		m_d;	//transparency 
	float		m_Tf; //transmission filter
	float4		m_ambientColor; //ambient color
	float4		m_diffuseColor; //diffuse color
	float4		m_specularColor; //specular color
	float4		m_emitColor; //emissive color
	float4		m_rimColor; //rimlight color
};

//not currently being used as a proper global light, its instead being used as a wierd ambient light source. needs fixing.
//scene global light settings
cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}

#define NUM_LIGHTS (20)

cbuffer LightPositionBuffer: register(b3)
{
	float4 m_light_pos[NUM_LIGHTS];
	float4 m_light_color[NUM_LIGHTS];
	float4 m_light_strength[NUM_LIGHTS];
};