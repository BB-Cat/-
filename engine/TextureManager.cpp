#include "TextureManager.h"
#include "Texture.h"
#include <iostream>


TextureManager::TextureManager(): ResourceManager()
{
}


TextureManager::~TextureManager()
{
}

TexturePtr TextureManager::createTextureFromFile(const wchar_t* file_path)
{
	return std::static_pointer_cast<Texture>(createResourceFromFile(file_path, nullptr));
}

Resource * TextureManager::createResourceFromFileConcrete(const wchar_t * file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode)
{
	Texture* tex = nullptr;
	try
	{
		tex = new Texture(file_path);
	}
	catch (int error) 
	{
		tex = nullptr;
		OutputDebugString(L"Texture could not be loaded.  Incorrect filename?\n");
	}

	return tex;
}
