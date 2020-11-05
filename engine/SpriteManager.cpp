#include "SpriteManager.h"
#include "Sprite.h"
#include "../include/DirectXTK-master/Inc/WICTextureLoader.h"


SpriteManager::SpriteManager() : ResourceManager()
{
}


SpriteManager::~SpriteManager()
{
}

SpritePtr SpriteManager::createSpriteFromFile(const wchar_t* file_path)
{
	return std::static_pointer_cast<Sprite>(createResourceFromFile(file_path, nullptr));
}

bool SpriteManager::loadShaderResourceView(ID3D11Device* device, const wchar_t* fileName, ID3D11ShaderResourceView** SRView, D3D11_TEXTURE2D_DESC* texDesc)
{
    //using DirectX::CreateWICTextureFromFile;

    //int no = -1;
    //ResourceShaderResourceViews* find = nullptr;
    //ID3D11Resource* resource = nullptr;

    //// データ探索
    //for (int n = 0; n < RESOURCE_MAX; n++)
    //{
    //    ResourceShaderResourceViews* p = &SRViews[n];//	エイリアス

    //    // データが無いなら無視
    //    // 但し、最初に見つけた領域ならセット用に確保
    //    if (p->refNum == 0)
    //    {
    //        if (no == -1) no = n;
    //        continue;
    //    }

    //    // ファイルパスが違うなら無視
    //    if (wcscmp(p->path, fileName) != 0) continue;

    //    // 同名ファイルが存在した
    //    find = p;
    //    p->SRView->GetResource(&resource);
    //    break;
    //}

    //// データが見つからなかった→新規読み込み
    //if (!find)
    //{
    //    ResourceShaderResourceViews* p = &SRViews[no];
    //    if (FAILED(CreateWICTextureFromFile(device, fileName, &resource, &(p->SRView))))
    //        return false;
    //    find = p;
    //    wcscpy_s(p->path, 256, fileName);
    //}

    //// 最終処理
    //ID3D11Texture2D* tex2D;
    //resource->QueryInterface(&tex2D);
    //*SRView = find->SRView;
    //tex2D->GetDesc(texDesc);
    //find->refNum++;
    //tex2D->Release();
    //resource->Release();
    return true;
}

Resource* SpriteManager::createResourceFromFileConcrete(const wchar_t* file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode)
{
	Sprite* spr = nullptr;
	try
	{
		spr = new Sprite(file_path);
	}
	catch (...) {}

	return spr;
}
