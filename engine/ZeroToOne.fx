inline float easeIn(float interpolator)
{
	return interpolator * interpolator;
}

float easeOut(float interpolator)
{
	return 1 - easeIn(1 - interpolator);
}

float easeInOut(float interpolator)
{
	float easeInValue = easeIn(interpolator);
	float easeOutValue = easeOut(interpolator);
	return lerp(easeInValue, easeOutValue, interpolator);
}

float3 easeInOut(float3 interpolator)
{
	float3 output;
	output.x = easeInOut(interpolator.x);
	output.y = easeInOut(interpolator.y);
	output.z = easeInOut(interpolator.z);
	return output;
}