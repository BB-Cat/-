
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

struct GSInput
{
	float4 position: POSITION0;
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 direction_to_camera : NORMAL1;
	float4 lightcolor: TEXCOORD2;
};

struct GSOutput
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 direction_to_camera : NORMAL1;
	float4 lightcolor: TEXCOORD2;
};

[maxvertexcount(3)]
[instance(1)]
void gsmain( triangle in GSInput vertexdata[3], uint instanceId: SV_GSInstanceID, inout TriangleStream<GSOutput> triStream )
{
	for (uint i = 0; i < 3; i++)
	{
		//GSOutput element;

		////float4 pos = vertexdata[i].position;

		////element.normal = mul(float4(vertexdata[i].normal.xyz, 0), m_global).xyz;

		//////////WORLD SPACE
		////pos = mul(mul(pos, m_world), m_global);
		////element.world_pos = pos;
		////element.direction_to_camera = normalize(pos.xyz - m_camera_position.xyz);
		////////VIEW SPACE
		////pos = mul(pos, m_view);
		////////SCREEN SPACE
		////pos = mul(pos, m_proj);
		////element.position = pos;


		//element.texcoord = vertexdata[i].texcoord;
		////element.direction_to_camera = vertexdata[i].direction_to_camera;
		//element.lightcolor = vertexdata[i].lightcolor;
		//triStream.Append(element);


		GSOutput element;
		element.position = vertexdata[i].position;
		element.texcoord = vertexdata[i].texcoord;
		element.world_pos = vertexdata[i].world_pos;
		element.normal = vertexdata[i].normal;
		element.direction_to_camera = vertexdata[i].direction_to_camera;
		element.lightcolor = vertexdata[i].lightcolor;
		triStream.Append(element);
	}
}