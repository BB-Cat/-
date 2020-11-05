#pragma once
#include "PixelShader.h"

class PixelShaderManager
{
public:
	PixelShaderManager();
	~PixelShaderManager();
	void compileShaders();
	void setPixelShader(int type);

private:
	//single texture lit pixel shader
	PixelShaderPtr m_pixel_shader_1tex;
	//no texture lit pixel shader
	PixelShaderPtr m_pixel_shader_notex;
};