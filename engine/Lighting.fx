

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


float3 hatchDiffuse(float3 normal, float3 light_dir, float3 light_color, float3 world_position, float time, float threshold)
{
	//use trilinear sampling based 70% on world position and 30% on a floored time modulo for that flickering effect
	//not sure how to do sampling within this function yet.

	//use the sample value to modify the threshold and make heavily hatched areas more likely to be shaded and unhatched areas less likely
	return 0;
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