#include "effect.fx"

struct PS_INPUT
{
	float4 position: SV_POSITION;
};

static const float	shadow_bias = 0.001;

float4 psmain(PS_INPUT input) : SV_TARGET
{
	return getFloat();
}