struct VS_INPUT
{
	float4 position: POSITION0;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float4 boneweights: WEIGHTS;
	uint4  boneindices: BONES;
};

struct VS_OUTPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD1;
	float3 normal: NORMAL0;
	float3 binormal: NORMAL1;
	float3 tangent: NORMAL2;
	float4 texbias: TEXCOORD2;
	float4 world_pos: TEXCOORD3;
	float cliff_amount : NORMAL3;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_camera_position;
};

#define MAX_BONES 32 
cbuffer constant: register(b1)
{
	row_major float4x4 m_global;
	row_major float4x4 m_bonetransforms[MAX_BONES];
	row_major float4x4 m_coordinate_system;
};

//scene global light settings
cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}

#define NUM_SHADOW_MAPS (3)
cbuffer constant: register(b3)
{
	float4 m_active_proj;
	row_major float4x4 m_light_world;
	row_major float4x4 m_light_view;
	row_major float4x4 m_light_proj[NUM_SHADOW_MAPS];
};


//bias value at which cliff texture begins to be added to result
static const float cliff_threshhold = 0.6f;
//value at which the result is fully a cliff
static const float max_cliff = 0.58f;

VS_OUTPUT vsmain(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = input.position;

	//--------------------------------------------------------------------------------------//
	//			screen space transformations												//
	//--------------------------------------------------------------------------------------//
	//WORLD SPACE
	output.world_pos = mul(mul(output.position, m_world), m_global);
	//VIEW SPACE
	output.position = mul(output.world_pos, m_view);
	//SCREEN SPACE
	output.position = mul(output.position, m_proj);

	//read the texture type from the input's boneweights
	float total_tex = input.boneweights.r + input.boneweights.g + input.boneweights.b;
	float r = input.boneweights.r / total_tex;
	float g = input.boneweights.g / total_tex;
	float b = input.boneweights.b / total_tex;
	output.texbias = float4(r, g, b, input.boneweights.w);

	output.normal = input.normal;

	//texture coordinates
	output.texcoord = float2((output.world_pos.x), (output.world_pos.z)) / 30;

	//if the texbias for this face is over the cliff threshhold, mix the cliff height
	output.cliff_amount = min(max((cliff_threshhold - input.boneweights.w) / (cliff_threshhold - max_cliff), 0), 1);

	return output;

}
