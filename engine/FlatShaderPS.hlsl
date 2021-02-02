Texture2D ShadowMapNear: register(t6);
Texture2D ShadowMapMid: register(t7);
Texture2D ShadowMapFar: register(t8);
SamplerState ShadowMapSampler: register(s6);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 shadowpos: TEXCOORD1;
	float3 shadowpos2: TEXCOORD2;
	float3 shadowpos3: TEXCOORD3;
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

static const float	shadow_bias = 0.001;

float4 psmain(PS_INPUT input) : SV_TARGET
{
	bool isInMap1 = (input.shadowpos.x > 0 && input.shadowpos.x <= 1.0 && input.shadowpos.y > 0 && input.shadowpos.y <= 1.0);
	bool isInMap2 = (input.shadowpos2.x > 0 && input.shadowpos2.x <= 1.0 && input.shadowpos2.y > 0 && input.shadowpos2.y <= 1.0);
	bool isInMap3 = (input.shadowpos3.x > 0 && input.shadowpos3.x <= 1.0 && input.shadowpos3.y > 0 && input.shadowpos3.y <= 1.0);


	float3 shadow = float3(0.2, 0.2, 0.2);
	float penumbra = 1.0;

	if (isInMap1 == true)
	{
		float shadowdepth = ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy).r;

		penumbra += 1.0f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0.003, 0)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0, 0.003)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(-0.003, 0)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0, -0.003)).r > shadow_bias);

		penumbra += 1.0f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0.0015, 0.0015)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0.0015, -0.0015)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(-0.0015, -0.0015)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(-0.0015, 0.0015)).r > shadow_bias);

		penumbra /= 8;
		//		 
		//penumbra -= 0.085f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0.005, 0)).r < shadow_bias);
		//penumbra -= 0.085f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0, 0.005)).r < shadow_bias);
		//penumbra -= 0.085f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(-0.005, 0)).r < shadow_bias);
		//penumbra -= 0.085f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0, -0.005)).r < shadow_bias);
		//		 
		//penumbra -= 0.04f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0.008, 0)).r < shadow_bias);
		//penumbra -= 0.04f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0, 0.008)).r < shadow_bias);
		//penumbra -= 0.04f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(-0.008, 0)).r < shadow_bias);
		//penumbra -= 0.04f * (input.shadowpos.z - ShadowMapNear.Sample(ShadowMapSampler, input.shadowpos.xy + float2(0, -0.008)).r < shadow_bias);

		shadow = (input.shadowpos.z - shadowdepth > shadow_bias) ? 1 - (shadow * penumbra) : 1.0;
	}
	else if (isInMap2 == true)
	{
		float shadowdepth2 = ShadowMapMid.Sample(ShadowMapSampler, input.shadowpos2.xy).r;

		penumbra += 1.0f * (input.shadowpos2.z - ShadowMapMid.Sample(ShadowMapSampler, input.shadowpos2.xy + float2(0.0015, 0)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos2.z - ShadowMapMid.Sample(ShadowMapSampler, input.shadowpos2.xy + float2(0, 0.0015)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos2.z - ShadowMapMid.Sample(ShadowMapSampler, input.shadowpos2.xy + float2(-0.0015, 0)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos2.z - ShadowMapMid.Sample(ShadowMapSampler, input.shadowpos2.xy + float2(0, -0.0015)).r > shadow_bias);

		penumbra /= 4;

		shadow = (input.shadowpos2.z - shadowdepth2 > shadow_bias) ? 1 - (shadow * penumbra) : 1.0;
	}
	else if (isInMap3 = true)
	{
		float shadowdepth3 = ShadowMapFar.Sample(ShadowMapSampler, input.shadowpos3.xy).r;

		penumbra += 1.0f * (input.shadowpos3.z - ShadowMapFar.Sample(ShadowMapSampler, input.shadowpos3.xy + float2(0.0015, 0)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos3.z - ShadowMapFar.Sample(ShadowMapSampler, input.shadowpos3.xy + float2(0, 0.0015)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos3.z - ShadowMapFar.Sample(ShadowMapSampler, input.shadowpos3.xy + float2(-0.0015, 0)).r > shadow_bias);
		penumbra += 1.0f * (input.shadowpos3.z - ShadowMapFar.Sample(ShadowMapSampler, input.shadowpos3.xy + float2(0, -0.0015)).r > shadow_bias);

		penumbra /= 4;

		shadow = (input.shadowpos3.z - shadowdepth3 > shadow_bias) ? 1 - (shadow * penumbra) : 1.0;
	}
	else
	{
		shadow = 1.0;
	}


	return float4(Kd * shadow, 1);
}