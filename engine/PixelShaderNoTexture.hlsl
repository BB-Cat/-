//Texture2D Texture: register(t0);
//SamplerState TextureSampler: register(s0);
//
//struct PS_INPUT
//{
//	float4 position: SV_POSITION;
//	float2 texcoord: TEXCOORD0;
//	float3 normal: NORMAL0;
//	float3 direction_to_camera: NORMAL1;
//	float3 world_pos: WORLD_POSITION;
//	float4 bonecolor: WEIGHTS;
//};
//
////world and camera settings
//cbuffer constant: register(b0)
//{
//	row_major float4x4 m_world;
//	row_major float4x4 m_view;
//	row_major float4x4 m_proj;
//};
//
////mesh lighting characteristics 
//cbuffer constant: register(b1)
//{
//	float	Ns; //specular power
//	float	Ni; //optical density
//	float	d;	//transparency 
//	float	Tr;   //transparency 
//	float	Tf; //transmission filter
//	float3	Ka; //ambient color
//	float3	Kd; //diffuse color
//	float3	Ks; //specular color
//	float	Ke; //emissive color
//};
//
////scene light settings
//cbuffer constant: register(b2)
//{
//	float3 m_light_dir;
//	float	m_light_strength;
//	float3 m_light_color;
//	float3 m_light_position;
//}
//
//float4 psmain(PS_INPUT input) : SV_TARGET
//{
//	float3 sample_color = Kd;
//
//	float3 ambient_light = m_light_color * m_light_strength;
//
//	float3 applied_light = m_light_color * input.bonecolor;
//
//	float3 vector_to_light = normalize(m_light_position - input.world_pos);
//
//	float3 diffuse_light_intensity = max(dot(vector_to_light, input.normal), 0);
//
//	float distance_to_light = distance(m_light_position, input.world_pos);
//
//	float attenuation_factor = 1 / (1.0f + 0.1f * distance_to_light + 0.1f * pow(distance_to_light, 2));
//
//	diffuse_light_intensity *= attenuation_factor;
//
//	float light_strength = 20.0f;
//	float3 light_color = float3(0.6f, 0.6f, 0.6f);
//
//	float3 diffuse_light = diffuse_light_intensity * light_strength;
//
//	applied_light += diffuse_light;
//
//	float3 final_color = applied_light * sample_color;
//
//
//	return float4(final_color, 1.0f);
//}
//
//
//float4 psterrain(PS_INPUT input) : SV_TARGET
//{
//	float3 sample_color = input.bonecolor;
//
//	float3 ambient_light = m_light_color * m_light_strength;
//
//	float3 applied_light = m_light_color;
//
//	float3 vector_to_light = normalize(m_light_position - input.world_pos);
//
//	float3 diffuse_light_intensity = max(dot(vector_to_light, input.normal), 0);
//
//	float distance_to_light = distance(m_light_position, input.world_pos);
//
//	float attenuation_factor = 1 / (1.0f + 0.1f * distance_to_light + 0.1f * pow(distance_to_light, 2));
//
//	diffuse_light_intensity *= attenuation_factor;
//
//	float light_strength = 20.0f;
//	float3 light_color = float3(0.6f, 0.6f, 0.6f);
//
//	float3 diffuse_light = diffuse_light_intensity * light_strength;
//
//	applied_light += diffuse_light;
//
//	float3 final_color = applied_light * sample_color;
//
//
//	return float4(final_color, 1.0f);
//}



////////
////////////////////////////////////////////////////////////////////////////
//#define NUMLIGHTS 10
//
////GLOBALS//
////world and camera settings
//cbuffer constant: register(b0)
//{
//	row_major float4x4 m_world;
//	row_major float4x4 m_view;
//	row_major float4x4 m_proj;
//	float4 m_camera_position;
//};
//
//#define MAX_BONES 32 
//cbuffer constant: register(b1)
//{
//	row_major float4x4 m_global;
//	row_major float4x4 m_bonetransforms[MAX_BONES];
//	row_major float4x4 m_coordinate_system;
//};
//
//cbuffer LightPositionBuffer
//{
//	float4 lightPosition[NUM_LIGHTS];
//	bool   is_lit;
//};
//
////mesh lighting characteristics 
//cbuffer constant: register(b1)
//{
//	float	Ns; //specular power
//	float	Ni; //optical density
//	float	d;	//transparency 
//	float	Tr;   //transparency 
//	float	Tf; //transmission filter
//	float3	Ka; //ambient color
//	float3	Kd; //diffuse color
//	float3	Ks; //specular color
//	float	Ke; //emissive color
//};
//
//#define MAX_BONES 32 
//cbuffer constant: register(b1)
//{
//	row_major float4x4 m_global;
//	row_major float4x4 m_bonetransforms[MAX_BONES];
//	row_major float4x4 m_coordinate_system;
//};
//
//
//
////////////////
//// TYPEDEFS //
////////////////
//struct VS_INPUT
//{
//	float4 position: POSITION0;
//	float2 tex: TEXCOORD0;
//	float3 normal: NORMAL0;
//	float4 boneweights: WEIGHTS;
//	uint4  boneindices: BONES;
//};
//
//struct PS_INPUT
//{
//	float4 position: SV_POSITION;
//	float2 tex: TEXCOORD0;
//	float3 normal: NORMAL0;
//	float3 direction_to_camera: NORMAL1;
//	float3 world_pos: WORLD_POSITION;
//	float4 bonecolor: WEIGHTS;
//};
/////////
//
//
//
//
//

//
//
//
//
//
//
///////VERTEX SHADER///////////////////////////////////////////////////////////
//
//PS_INPUT vsmain(VS_INPUT input)
//{
//
//	PS_INPUT output = (PS_INPUT)0;
//	output.texcoord = input.texcoord;
//
//	//float3 p = { 0, 0, 0 };
//	//float3 n = { 0, 0, 0 };
//	//int i = 0;
//	//for (i = 0; i < 4; i++)
//	//{
//	//	p += (input.boneweights[i] * mul(input.position, m_bonetransforms[input.boneindices[i]])).xyz;
//	//	n += (input.boneweights[i] * mul(float4(input.normal.xyz, 0), m_bonetransforms[input.boneindices[i]])).xyz;
//	//}
//	//if (p.x != 0 || p.y != 0 || p.z != 0)
//	//{
//	//	output.position = float4(p, 1.0f);
//	//	output.normal = float4(n, 0.0f);
//	//}
//	//else
//	//{
//	//	output.position = input.position;
//	//	output.normal = input.normal;
//	//}
//
//
//	////WORLD SPACE
//	//output.position = mul(mul(output.position, m_world), m_global);
//	//output.world_pos = output.position;
//	//output.direction_to_camera = normalize(output.position.xyz - m_camera_position.xyz);
//	////VIEW SPACE
//	//output.position = mul(output.position, m_view);
//	////SCREEN SPACE
//	//output.position = mul(output.position, m_proj);
//
//
//
//	//// -- not being used currently
//	//float4 influence = { 1,1,1,1 };
//	//output.bonecolor = influence;
//	//// -- not being used currently
//
//
//	//return output;
//	return output;
//
//}
//
///////PIXEL SHADER///////////////////////////////////////////////////////
//
//Texture2D Texture: register(t0);
//SamplerState TextureSampler: register(s0);
//
////scene light settings
//cbuffer constant: register(b2)
//{
//	float3 m_light_dir;
//	float	m_light_strength;
//	float3 m_light_color;
//	float3 m_light_position;
//}
//
//float4 psmain(PS_INPUT input) : SV_TARGET
//{
//	//float3 sample_color = Kd;
//
//	//float3 ambient_light = m_light_color * m_light_strength;
//
//	//float3 applied_light = m_light_color * input.bonecolor;
//
//	//float3 vector_to_light = normalize(m_light_position - input.world_pos);
//
//	//float3 diffuse_light_intensity = max(dot(vector_to_light, input.normal), 0);
//
//	//float distance_to_light = distance(m_light_position, input.world_pos);
//
//	//float attenuation_factor = 1 / (1.0f + 0.1f * distance_to_light + 0.1f * pow(distance_to_light, 2));
//
//	//diffuse_light_intensity *= attenuation_factor;
//
//	//float light_strength = 20.0f;
//	//float3 light_color = float3(0.6f, 0.6f, 0.6f);
//
//	//float3 diffuse_light = diffuse_light_intensity * light_strength;
//
//	//applied_light += diffuse_light;
//
//	//float3 final_color = applied_light * sample_color;
//
//
//	//return float4(final_color, 1.0f);
//	return float4(Kd, 1.0f);
//}