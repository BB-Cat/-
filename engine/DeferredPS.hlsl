Texture2D Texture: register(t0);
SamplerState TextureSampler: register(s0);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 direction_to_camera : NORMAL1;
	float4 lightcolor: TEXCOORD2;
};

struct PS_OUTPUT
{
	float4 color: SV_TARGET0;
	float3 light: SV_TARGET1;
	float3 normal: SV_TARGET2;
	float4 shadow: SV_TARGET3; //unused test
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

PS_OUTPUT psmain(PS_INPUT input)
{
	PS_OUTPUT output;
	//diffuse
	float atten = 1.0;
	float3 lightDir = normalize(m_global_light_dir.xyz);
	float3 diffuseReflection = atten * m_global_light_color.xyz * max(0.0, dot(input.normal, lightDir));

	//ambient lighting
	float3 ambientLightDir = normalize(m_global_light_dir.xyz) * -1;
	float3 ambientReflection = 0.5 * m_ambient_light_color.xyz * max(0.0, dot(input.normal, ambientLightDir));

	//specular
	float3 specularReflection = m_specularColor.rgb * max(0.0, dot(input.normal, lightDir.xyz))
		* pow(max(0.0, dot(reflect(-lightDir.xyz, input.normal), -input.direction_to_camera)), m_shininess);

	//rim lighting
	float rim = 1 - saturate(dot(-input.direction_to_camera, input.normal));
	float rimlight_amount = max(dot(lightDir, input.normal), 0);
	float3 rimLighting = m_rimColor.w * atten * m_global_light_color.rgb * m_rimColor.xyz * rimlight_amount * pow(rim, m_rimPower);


	float3 lightFinal = rimLighting + diffuseReflection + specularReflection + ambientReflection + m_ambient_light_color.rgb;
	float3 sample_color = Texture.Sample(TextureSampler, input.texcoord);

	//return float4(sample_color * lightFinal, m_d);

	output.color = float4(sample_color, 1);
	output.light = lightFinal;
	output.normal = input.normal;
	output.shadow = float4(0.0, 0.0, 0.0, 1.0);

	return output;

}