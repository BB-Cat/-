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
	return light_color * (0.5 + max(0.0, dot(normal, light_dir) * 0.5);
}



float3 oneCellDiffuse(float3 normal, float3 light_dir, float3 light_color, float threshold)
{
	if (max(0.0, dot(normal, light_dir)) > threshold) return light_color;
	return 0;
}



float3 gradientOneCellDiffuse(float3 normal, float3 light_dir, float3 light_color, float threshold)
{
	return lerp(gradientDiffuse(normal, light_dir, light_color), oneCellDiffuse(normal, light_dir, light_color, threshold), 0.35);
}


float3 hatchDiffuse(float3 normal, float3 light_dir, float3 light_color, float3 world_position, float time, float threshold, Texture2D tex)
{
	//get the sample positions
	float3 texpos = world_position + 0.7 * float3(floor((time * 9.0) % 3.0));
	//trilinear sampling
	float xy_sample = tex.Sample(LightingSampler, texpos.xy * 0.5).r;
	float xz_sample = tex.Sample(LightingSampler, texpos.xz).r;
	float yz_sample = tex.Sample(LightingSampler, texpos.yz).r;
	//blend the values together
	float3 normal_blend = abs(normal);
	float hatch = xy_sample * normal_blend.z + xz_sample * normal_blend.y + yz_sample * normal_blend.x;
	//add hatch if over threshhold of the light dot
	float dotL = max(0.0, dot(normal, light_dir));
	hatch = step(-0.5 + hatch * 1.0, dotL);
	//add gradient for smoother color
	float3 gradient = light_color * (0.5 + dotL * 0.5);
	//mix them together
	float3 color = gradient * 0.5 + float3(hatch * light_color.rgb) * 0.5;
	//return the result
	return color;
}



float3 spec(float3 normal, float3 light_dir, float3 spec_color float3 dir_to_camera, float shininess)
{
	return spec_color * max(0, dot(input.normal, light_dir)) * 
		pow(max(0, dot(reflect(-light_dir, normal), -dir_to_camera)), shininess);
}



float3 oneCellSpec(float3 normal, float3 light_dir, float3 spec_color float3 dir_to_camera, float shininess, float threshold)
{
	float value = max(0, dot(input.normal, light_dir)) *
		pow(max(0, dot(reflect(-light_dir, normal), -dir_to_camera)), shininess);
	if (value > threshold) return spec_color;
	return 0;
}