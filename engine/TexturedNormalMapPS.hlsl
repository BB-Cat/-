
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
	float3 vx = normalize(input.tangent);
	float3 vy = normalize(input.binormal);
	float3 vz = normalize(input.normal);

	float3x3 inverseMatrix = { vx,vy,vz };
	float3 inverseToCamera = normalize(mul(inverseMatrix, input.direction_to_camera));

	float3 normal = NormalTexture.Sample(NormalSampler, input.texcoord).xyz + float3(0,0,0.5f);
	float3x3 mat = { vx,vy,vz };
	normal = normalize(mul(normal, mat));

	//diffuse
	float atten = 1.0;
	float3 lightDir = normalize(m_global_light_dir.xyz);
	float3 diffuseReflection = atten * m_global_light_color.xyz * max(0.0, dot(normal, lightDir));

	//ambient lighting
	float3 ambientLightDir = normalize(m_global_light_dir.xyz) * -1;
	float3 ambientReflection = 0.5 * m_ambient_light_color.xyz * max(0.0, dot(normal, ambientLightDir));

	//specular
	float3 specularReflection = m_specularColor.rgb * max(0.0, dot(normal, m_global_light_dir.xyz))
		* pow(max(0.0, dot(reflect(-lightDir.xyz, normal), -input.direction_to_camera)), m_shininess);

	//rim lighting
	float rim = 1 - saturate(dot(-input.direction_to_camera, normal));
	float rimlight_amount = max(dot(lightDir, normal), 0);
	float3 rimLighting = m_rimColor.w * atten * m_global_light_color.rgb * m_rimColor.xyz * rimlight_amount * pow(rim, m_rimPower);


	float3 lightFinal = rimLighting + diffuseReflection + specularReflection + ambientReflection + m_ambient_light_color.rgb;
	float3 sample_color = Texture.Sample(TextureSampler, input.texcoord);

	return float4(sample_color * lightFinal, m_d);
}