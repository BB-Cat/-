#pragma once
#include "ResourceManager.h"
#include <d3d11.h>

class SpriteManager : public ResourceManager
{
public:
	SpriteManager();
	~SpriteManager();
	SpritePtr createSpriteFromFile(const wchar_t* file_path);

	static bool loadShaderResourceView(ID3D11Device* device, const wchar_t* fileName,
		ID3D11ShaderResourceView** SRView, D3D11_TEXTURE2D_DESC* texDesc);
protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path, bool is_flipped, float* topology = nullptr, D3D11_CULL_MODE cullmode = D3D11_CULL_BACK);
};

