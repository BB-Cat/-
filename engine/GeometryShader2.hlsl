
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

[maxvertexcount(9)]
void gsmain(triangle in GSInput vertexdata[3], inout TriangleStream<GSOutput> triStream)
{
	GSOutput ref[3];
	
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;

		float4 pos = vertexdata[i].position;

		element.normal = mul(float4(vertexdata[i].normal.xyz, 0), m_global).xyz;

		//////WORLD SPACE
		pos = mul(mul(pos, m_world), m_global);

		float avg = (vertexdata[i].lightcolor.x);
		pos = pos + float4((element.normal * 2).xyz, 0) * max(sin(avg * 2), 0) * (i == 1);
		element.world_pos = pos;
		element.direction_to_camera = normalize(pos.xyz - m_camera_position.xyz);
		////VIEW SPACE
		pos = mul(pos, m_view);
		////SCREEN SPACE
		pos = mul(pos, m_proj);
		element.position = pos;

		//element.position = vertexdata[i].position;
		element.texcoord = vertexdata[i].texcoord;
		//element.world_pos = vertexdata[i].world_pos;
		//element.normal = vertexdata[i].normal;
		element.direction_to_camera = vertexdata[i].direction_to_camera;
		element.lightcolor = vertexdata[i].lightcolor;
		//triStream.Append(element);

		ref[i] = element;
	}


	triStream.Append(ref[0]);
	triStream.Append(ref[1]);
	triStream.Append(ref[2]);
	
	
	//GSOutput final;
	//final.position = (ref[0].position + ref[1].position + ref[2].position) / 3.0f;
	//final.texcoord = (ref[0].texcoord + ref[1].texcoord + ref[2].texcoord) / 3.0f;
	//final.world_pos = (ref[0].world_pos + ref[1].world_pos + ref[2].world_pos) / 3.0f;
	//final.normal = (ref[0].normal + ref[1].normal + ref[2].normal) / 3.0f;
	//final.direction_to_camera = (ref[0].direction_to_camera + ref[1].direction_to_camera + ref[2].direction_to_camera) / 3.0f;
	//final.lightcolor = (ref[0].lightcolor + ref[1].lightcolor + ref[2].lightcolor) / 3.0f;
	//triStream.Append(final);


}