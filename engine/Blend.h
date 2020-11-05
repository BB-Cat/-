#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Prerequisites.h"


enum BlendType
{
	ALPHA,
	ADD,
	SUBTRACT,
	REPLACE,
	MULTIPLY,
	LIGHTEN,
	DARKEN,
	SCREEN,
	ALPHA_TO_COVERAGE,
};

class BlendMode
{
public:
	static BlendMode* get();
	void SetBlend(int mode);

private:
	static BlendMode* b;
	BlendMode();
	ID3D11BlendState	*m_blend_state[9];
private:
	friend class DeviceContext;
};