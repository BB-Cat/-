#pragma once
#include "ComputeShader.h"

class TextureComputeShader : public ComputeShader
{
public:
	TextureComputeShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system,
		Vec2 dimensions);
	~TextureComputeShader();


	//functions for applying textures or other resource views after setting a compute shader and before running it
	//Do NOT use if you will use the normal runComputeShader!
	void setComputeShader();
	void runComputeShaderNoSet();

	void runComputeShader() override;

	ID3D11ShaderResourceView* getTexture();

private:
	ID3D11Texture2D* m_dynamictex = nullptr;
	Vec2 m_dimensions;
};