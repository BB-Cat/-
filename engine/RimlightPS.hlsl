struct PS_INPUT
{
	float4 position: SV_POSITION;
	float4 world_pos : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 direction_to_camera : NORMAL1;
	float4 lightcolor: TEXCOORD1;
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
	
	//diffuse
	float atten = 1.0;
	float3 lightDir = normalize(m_global_light_dir.xyz);
	float3 diffuseReflection = atten * m_global_light_color.xyz * max(0.0, dot(input.normal, lightDir));

	//ambient lighting
	float3 ambientLightDir = normalize(m_global_light_dir.xyz) * -1;
	float3 ambientReflection = 0.5 * m_ambient_light_color.xyz * max(0.0, dot(input.normal, ambientLightDir));
	
	//specular
	float3 specularReflection = min(1.0, m_specularColor.rgb * max(0.0, dot(input.normal, m_global_light_dir.xyz))
		* pow(max(0.0, dot(reflect(-lightDir.xyz, input.normal), -input.direction_to_camera)), m_shininess)) * m_specularColor.a;

	//rim lighting
	float rim = 1 - saturate(dot(-input.direction_to_camera, input.normal));
	float rimlight_amount = max(dot(lightDir, input.normal), 0);
	float3 rimLighting = m_rimColor.w * atten * m_global_light_color.rgb * m_rimColor.xyz * rimlight_amount * pow(rim, m_rimPower);

	
	float3 lightFinal = rimLighting + diffuseReflection + specularReflection + ambientReflection;
	//float3 lightFinal = specularReflection;
	
	return float4(m_diffuseColor.xyz * lightFinal, m_d);
	//return float4(rimLighting + specularReflection, m_d);
	//return float4(ambientReflection + specularReflection, m_d);
}