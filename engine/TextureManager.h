#pragma once
#include "ResourceManager.h"


class TextureManager: public ResourceManager
{
public:
	TextureManager();
	~TextureManager();
	TexturePtr createTextureFromFile(const wchar_t* file_path);
protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode = D3D11_CULL_BACK);
};

