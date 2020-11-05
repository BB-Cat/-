Texture2D Texture: register(t0);
SamplerState TextureSampler: register(s0);

Texture2D NormalTexture: register(t1);
SamplerState NormalSampler: register(s1);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 binormal : NORMAL1;
	float3 tangent : NORMAL2;
	float3 direction_to_camera : NORMAL3;
	float4 lightcolor: TEXCOORD2;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_camera_position;
};

//mesh lighting characteristics 
cbuffer constant: register(b1)
{
	float		m_shininess; //specular power
	float		m_rimPower; //rimlight power
	float		m_d;	//transparency 
	float		m_metallicAmount; //amount of metallicism
	float4		m_ambientColor; //ambient color
	float4		m_diffuseColor; //diffuse color
	float4		m_specularColor; //specular color
	float4		m_emitColor; //emissive color
	float4		m_rimColor; //rimlight color
};

//scene global light settings
cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{

	//normal calculation
		// ê⁄ãÛä‘é≤
	//float3 vx = normalize(input.tangent);
	//float3 vy = normalize(input.binormal);
	//float3 vz = normalize(input.normal);

	//// éãê¸ê⁄ãÛä‘ïœä∑
	//float3x3 inverseMatrix = { vx,vy,vz };
	//float3 inverseToCamera = normalize(mul(inverseMatrix, input.direction_to_camera));

	//float3 normal = NormalTexture.Sample(NormalSampler, input.texcoord).xyz;
	//normal.x = (normal.x * 2) - 1;
	//float temp = normal.y;
	//normal.y = 0.5f;
	//normal.z = temp;
	//normal.z = (normal.z * 2) - 1;
	//normal = normalize(normal);

	//normal = float3(0, 0, -1);
	//float3x3 mat = { vx,vy,vz };
	//normal = normalize(mul(normal, mat));



	////float3 normal = input.normal;
	//float3 normal = float3(0,1,0);
	//float3 vx = normalize(float3(normal.z, normal.x, normal.y));
	//float3 vy = normalize(float3(normal.y, normal.z, normal.x));
	//float3 vz = normalize(normal);

	//// éãê¸ê⁄ãÛä‘ïœä∑
	//float3x3 inverseMatrix = { vx,vy,vz };
	//float3 inverseToCamera = normalize(mul(inverseMatrix, input.direction_to_camera.xyz));

	//normal = NormalTexture.Sample(NormalSampler, input.texcoord).xyz;
	//normal = normal * 2 - 1;
	//float3x3 mat = { vx,vy,vz };
	//normal = normalize(mul(normal, mat));




	float3 normal = NormalTexture.Sample(NormalSampler, input.texcoord).xyz;
	normal = normal * 2 - 1;
	float temp = normal.z;
	normal.z = normal.y;
	normal.y = temp;

	//diffuse
	float atten = 1.0;
	float3 lightDir = normalize(m_global_light_dir.xyz);
	float3 diffuseReflection = atten * m_global_light_color.xyz * max(0.0, dot(normal, lightDir));

	//ambient lighting
	float3 ambientLightDir = normalize(m_global_light_dir.xyz) * -1;
	float3 ambientReflection = 0.5 * m_ambient_light_color.xyz * max(0.0, dot(normal, ambientLightDir));


	float3 direction_to_camera = normalize(input.world_pos.xyz - m_camera_position.xyz);

	//specular
	float3 specularReflection = m_specularColor.rgb * max(0.0, dot(normal, lightDir.xyz))
		* pow(max(0.0, dot(reflect(-lightDir.xyz, normal), -direction_to_camera)), 1);

	//rim lighting
	float rim = 1 - saturate(dot(-direction_to_camera, normal));
	float rimlight_amount = max(dot(lightDir, normal), 0);
	float3 rimLighting = m_rimColor.w * atten * m_global_light_color.rgb * m_rimColor.xyz * rimlight_amount * pow(rim, m_rimPower);


	float3 lightFinal = rimLighting + diffuseReflection + specularReflection + ambientReflection + m_ambient_light_color.rgb;
	float3 sample_color = Texture.Sample(TextureSampler, input.texcoord);

	return float4(lightFinal * sample_color, m_d);
}