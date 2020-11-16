#pragma once
#include "Prerequisites.h"
#include <string>

class Shader
{
public:
	Shader(const wchar_t* vs_file, const wchar_t* ps_file, void* shader_byte_code, size_t& size_shader);

	void compile(const wchar_t* vs_file, const wchar_t* ps_file, void* shader_byte_code, size_t& size_shader);

	bool recompile(ShaderPtr error_shader);
	bool ifErrorRecompile(ShaderPtr error_shader);

	void ifErrorReplaceShaders(VertexShaderPtr err_vs, PixelShaderPtr err_ps);
	


	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;

	std::wstring m_vsname;
	std::wstring m_psname;

	bool m_initiated = false;
};