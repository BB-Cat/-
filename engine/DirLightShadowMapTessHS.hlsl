
// ���͐���_
struct VS_CONTROL_POINT_OUTPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal : NORMAL0;
	float tessfactor : TESSELATION;
};

// �o�͐���_
struct HS_CONTROL_POINT_OUTPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal : NORMAL0;
};

// �o�̓p�b�`�萔�f�[�^�B
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // ���Ƃ��΁A�N���h �h���C���� [4] �ɂȂ�܂�
	float InsideTessFactor : SV_InsideTessFactor; // ���Ƃ��΁A�N���h �h���C���� Inside[2] �ɂȂ�܂�
	// TODO: ���̃X�^�b�t�̕ύX/�ǉ�
};


cbuffer constant: register(b0)
{
	float		m_tess_factor;
};




#define NUM_CONTROL_POINTS 3

// �p�b�`�萔�֐�
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	//// �����ɃR�[�h��}�����ďo�͂��v�Z���܂�
	//Output.EdgeTessFactor[0] = m_tess_factor;
	//Output.EdgeTessFactor[1] = m_tess_factor;
	//Output.EdgeTessFactor[2] = m_tess_factor;
	//Output.InsideTessFactor = m_tess_factor; // ���Ƃ��΁A����ɓ��I�e�Z���[�V�����W�����v�Z�ł��܂�

	Output.EdgeTessFactor[0] = 0.5f * (ip[1].tessfactor + ip[2].tessfactor);
	Output.EdgeTessFactor[1] = 0.5f * (ip[2].tessfactor + ip[0].tessfactor);
	Output.EdgeTessFactor[2] = 0.5f * (ip[0].tessfactor + ip[1].tessfactor);
	Output.InsideTessFactor = Output.EdgeTessFactor[0];

	return Output;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT hsmain(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONTROL_POINT_OUTPUT Output;

	// �����ɃR�[�h��}�����ďo�͂��v�Z���܂�
	Output.position = ip[i].position;
	Output.texcoord = ip[i].texcoord;
	Output.normal = ip[i].normal;

	return Output;
}
