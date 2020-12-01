SamplerState LightingSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


float3 diffuse(float3 normal, float3 light_dir, float3 light_color)
{
	return light_color * max(0.0, dot(normal, light_dir));
}



float3 gradientDiffuse(float3 normal, float3 light_dir, float3 light_color)
{
	return light_color * 0.3 + max(0.0, dot(normal, light_dir) * 0.7);
}



float3 oneCellDiffuse(float3 normal, float3 light_dir, float3 light_color, float threshold)
{
	if (max(0.0, dot(normal, light_dir)) > threshold) return light_color;
	return light_color * 0.5;
}

float3 cellStroke(float3 normal, float3 eye_dir, float3 diffuse, float3 stroke_color, float threshold)
{
	if (1.0 - max(0.0, dot(normal, eye_dir)) > threshold) return stroke_color;
	return diffuse;
}



float3 gradientOneCellDiffuse(float3 normal, float3 light_dir, float3 light_color, float threshold)
{
	return lerp(gradientDiffuse(normal, light_dir, light_color), oneCellDiffuse(normal, light_dir, light_color, threshold), 0.4);
}


float3 hatchDiffuse(float3 normal, float3 light_dir, float3 light_color, float3 world_position, float time, float threshold, Texture2D tex)
{
	////get the sample positions
	//float3 texpos = world_position + 0.7 * float3(floor((time * 9.0) % 3.0));
	////trilinear sampling
	//float xy_sample = tex.Sample(LightingSampler, texpos.xy * 0.5).r;
	//float xz_sample = tex.Sample(LightingSampler, texpos.xz).r;
	//float yz_sample = tex.Sample(LightingSampler, texpos.yz).r;
	////blend the values together
	//float3 normal_blend = abs(normal);
	//float hatch = xy_sample * normal_blend.z + xz_sample * normal_blend.y + yz_sample * normal_blend.x;
	////add hatch if over threshhold of the light dot
	//float dotL = max(0.0, dot(normal, light_dir));
	//hatch = step(-0.5 + hatch * 1.0, dotL);
	////add gradient for smoother color
	//float3 gradient = light_color * (0.5 + dotL * 0.5);
	////mix them together
	//float3 color = gradient * 0.5 + float3(hatch * light_color.rgb) * 0.5;
	////return the result
	//return color;

	return 0;
}



float3 spec(float3 normal, float3 light_dir, float3 spec_color, float3 dir_to_camera, float shininess)
{
	return spec_color * max(0.0, dot(normal, light_dir)) * 
		pow(max(0.0, dot(reflect(-light_dir, normal), -dir_to_camera)), shininess);
}



float3 oneCellSpec(float3 normal, float3 light_dir, float3 spec_color, float3 dir_to_camera, float shininess, float threshold)
{
	float value = max(0, dot(normal, light_dir)) *
		pow(max(0, dot(reflect(-light_dir, normal), -dir_to_camera)), shininess);
	if (value > threshold) return spec_color;
	return 0;
}



float rim(float3 normal, float3 light_dir, float3 rim_color, float3 dir_to_camera, float power)
{

	//float rim = 1 - saturate(dot(-input.direction_to_camera, input.normal));
	//float rimlight_amount = max(dot(lightDir, input.normal), 0);
	//float3 rimLighting = m_rimColor.w * atten * m_global_light_color.rgb * m_rimColor.xyz * rimlight_amount * pow(rim, m_rimPower);

	float rim_level = pow(1.0 - max(0.0, dot(normal, -dir_to_camera)), power);
	float rim_diffuse = max(dot(light_dir, normal), 0);

	return rim_color * rim_level * rim_diffuse;

	//return rim_color * (1.0 - max(0.0, dot(normal, light_dir))) *
	//	pow(max(0.0, dot(reflect(-light_dir, normal), -dir_to_camera)), power);
}