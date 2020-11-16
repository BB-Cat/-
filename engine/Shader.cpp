#include "Shader.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"

Shader::Shader(const wchar_t* vs_file, const wchar_t* ps_file, void* shader_byte_code, size_t& size_shader)
{
	//save the file name so it can be recompiled later
	m_vsname = vs_file;
	m_psname = ps_file;

	compile(vs_file, ps_file, shader_byte_code, size_shader);
}

void Shader::compile(const wchar_t* vs_file, const wchar_t* ps_file, void* shader_byte_code, size_t& size_shader)
{
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(vs_file, "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(ps_file, "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	if (m_vs != nullptr && m_ps != nullptr) m_initiated = true;
}

bool Shader::recompile(ShaderPtr error_shader)
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(m_vsname.c_str(), "vsmain", &shader_byte_code, &size_shader);
	VertexShaderPtr vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(m_psname.c_str(), "psmain", &shader_byte_code, &size_shader);
	PixelShaderPtr ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	if (ps != nullptr && vs != nullptr)
	{
		m_vs = vs;
		m_ps = ps;
		m_initiated = true;
		return true;
	}
	else
	{
		m_initiated = false;
		m_vs = error_shader->m_vs;
		m_ps = error_shader->m_ps;
	}

	return false;
}

bool Shader::ifErrorRecompile(ShaderPtr error_shader)
{
	if (m_initiated) return true;
	else return recompile(error_shader);
}

void Shader::ifErrorReplaceShaders(VertexShaderPtr err_vs, PixelShaderPtr err_ps)
{
	if (m_vs == nullptr || m_ps == nullptr)
	{
		m_vs = err_vs;
		m_ps = err_ps;
	}
}