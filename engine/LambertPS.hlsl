struct PS_INPUT
{
	float4 position: SV_POSITION;
	float4 lightcolor: TEXCOORD0;
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

float4 psmain(PS_INPUT input) : SV_TARGET
{
	return float4(m_diffuseColor.xyz * input.lightcolor, m_d);
}