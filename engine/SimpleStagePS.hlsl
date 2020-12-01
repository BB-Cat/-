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

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_camera_position;
};

//scene global light settings
cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}

static const float ring_range = 100.0;
static const float height1 = -20;
static const float height2 = -40;

static const float	shadow_bias = 0.001;

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float steps_to_height1 = height1 / -input.direction_to_camera.y;
	float steps_to_height2 = height2 / -input.direction_to_camera.y;

	float3 pos1 = input.world_pos;
	float3 pos2 = input.world_pos + steps_to_height1 * -input.direction_to_camera;
	float3 pos3 = input.world_pos + steps_to_height2 * -input.direction_to_camera;

	float len1 = length(pos1);
	float len2 = length(pos2);
	float len3 = length(pos3);

	float4 color1 = float4(0.6, 0.4, 0.2, 1.0) * (max((ring_range - len1) / ring_range, 0) * 1.0);
	color1 *= (color1.w > 0.2);
	float4 color2 = float4(0.8, 0.8, 0.1, 1.0) * (max((ring_range - len2) / ring_range, 0) * 1.0);
	color2 *= (color2.w > 0.2);
	float4 color3 = float4(0.4, 0.4, 0.0, 1.0) * (max((ring_range - len3) / ring_range, 0) * 1.0);
	color3 *= (color3.w > 0.2);

	float edge_light_amount = pow(1.0 - max((ring_range * 0.8 - len1) / (ring_range * 0.8), 0), 15);
	float3 edge_light = float3(1, 1, 1);
	edge_light_amount *= (color1.w > 0.2);

	//specular
	float3 normal = float3(0, 1, 0);
	float3 dir = normalize(m_camera_position.xyz - pos1);
	float3 lightDir = normalize(m_global_light_dir.xyz);
	float shininess = 15;
	float specular = max(0.0, dot(normal, lightDir.xyz))
		* pow(max(0, dot(reflect(-lightDir.xyz, normal), dir)), shininess);
	float4 spec_color = float4(0.2, 0.2, 0.2, 0.2);

	float mod = (floor(len1 * 3) % 30 == 0 && !(len1 > ring_range * 0.8));
	specular *= mod;

	float4 tiles = float4(-0.2, -0.2, -0.2, 0) * (floor(pos1.x * 3) % 20 != 0 && floor(pos1.z * 3) % 20 != 0);

	float4 final = (color1 + color2 + color3 + tiles) + float4(edge_light, 1) * edge_light_amount + spec_color * specular;



	final -= float4(0.2, 0.2, 0.2, 0.2) * max(0, dot(reflect(-lightDir.xyz, normal), -dir)) * mod;



	bool isInMap1 = (input.shadowpos.x > 0 && input.shadowpos.x <= 1.0 && input.shadowpos.y > 0 && input.shadowpos.y <= 1.0);
	bool isInMap2 = (input.shadowpos2.x > 0 && input.shadowpos2.x <= 1.0 && input.shadowpos2.y > 0 && input.shadowpos2.y <= 1.0);
	bool isInMap3 = (input.shadowpos3.x > 0 && input.shadowpos3.x <= 1.0 && input.shadowpos3.y > 0 && input.shadowpos3.y <= 1.0);

	float shadow = 0.2;
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

	return final * shadow;
	//return mod;
	//return float4(specular, 1);
}