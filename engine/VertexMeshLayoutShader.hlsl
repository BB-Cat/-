//this file is used during initialization to store the byte value and width so that the mesh classes can initialize correctly.  
//if a change is made to the VSINPUT or VSOUTPUT used in runtime shaders the values must be changed here AS WELL!!!

struct VS_INPUT
{
	float4 position: POSITION0;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float4 boneweights: WEIGHTS;
	uint4  boneindices: BONES;
};

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: NORMAL1;
	float3 world_pos: WORLD_POSITION;
};

PS_INPUT vsmain(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	return output;
}