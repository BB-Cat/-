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
	float4 texbias: TEXCOORD2;
	float3 light: NORMAL0;
	float cliff_amount : NORMAL1;
	float3 fog_color: TEXCOORD3;
	float fog_amount : NORMAL2;
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

//terrain colors
cbuffer constant: register(b4)
{
	float4 color1; //color averages for each type of terrain
	float4 color2; //color averages for each type of terrain
	float4 color3; //color averages for each type of terrain
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
	output.position = mul(mul(output.position, m_world), m_global);
	float3 direction_to_camera = normalize(output.position - m_camera_position.xyz);
	//texture coordinates
	output.texcoord = float2((output.position.x), (output.position.z)) / 10;
	//VIEW SPACE
	output.position = mul(output.position, m_view);
	//SCREEN SPACE
	output.position = mul(output.position, m_proj);


	//--------------------------------------------------------------------------------------//
	//lighting calculations - done per vertex instead of per pixel to reduce processing cost//
	//--------------------------------------------------------------------------------------//
	//diffuse lighting
	float3 light_direction = normalize(m_global_light_dir.xyz);
	float3 diffuseReflection = m_global_light_strength * m_global_light_color.xyz * max(0.0, dot(input.normal, light_direction));

	////ambient lighting
	//float3 ambientLightDir = light_direction * -1;
	//float ambientAmount = max(0.0, dot(input.normal, ambientLightDir)) / 2.0 + 0.5;
	//float3 ambientReflection = 0.5 * m_ambient_light_color.rgb * ambientAmount;

	//specular
	float3 specularReflection = float3(0.2f, 0.4f, 0.4f) * max(0.0, dot(input.normal, light_direction))
		* pow(max(0.0, dot(reflect(-light_direction, input.normal), -direction_to_camera)), 12);

	//rim lighting
	float rim = 1 - saturate(dot(-direction_to_camera, input.normal));
	float rimlight_amount = max(dot(light_direction, input.normal), 0);
	float3 rimLighting = m_global_light_strength * m_global_light_color.rgb * float3(0.4f, 0.7f, 0.7f) * rimlight_amount * pow(rim, 2);

	output.light = diffuseReflection + m_ambient_light_color + specularReflection + rimLighting;
	//output.light = diffuseReflection + ambientReflection;

	//read the texture type from the input's boneweights
	float total_tex = input.boneweights.r + input.boneweights.g + input.boneweights.b;
	float r = input.boneweights.r / total_tex;
	float g = input.boneweights.g / total_tex;
	float b = input.boneweights.b / total_tex;
	output.texbias = float4(r, g, b, input.boneweights.w);

	//if the texbias for this face is over the cliff threshhold, mix the cliff height
	output.cliff_amount = min(max((cliff_threshhold - input.boneweights.w) / (cliff_threshhold - max_cliff), 0), 1);

	return output;

}
