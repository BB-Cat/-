#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class Sampler
{
public:
	Sampler(RenderSystem* system);
	~Sampler();
private:
	ID3D11SamplerState* m_sampler;
private:
	friend class RenderSystem;
	friend class DeviceContext;
};