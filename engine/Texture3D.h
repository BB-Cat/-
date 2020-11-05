#pragma once
#include "Resource.h"
#include <d3d11.h>
#include "Vector2D.h"
#include "Vector3D.h"

#include <vector>

/*
こちらは3次元テクスチャです。
現在３Dテクスチャの使い道として雲のボリューメトリック計算にしか使う予定がない為、
別のリソースと違って専用のリソースマネジャーが存在しません。

This is the 3D texture class.
It won't be used for anything besides volumetric rendering (for now)
so it does not have a resource manager counterpart.  If 3D textures become useful for other things
in the future I will make a manager then.
*/

class Texture3D
{
public:
	//計算済みのデーターを用いてテクスチャーを作る
	Texture3D(Vector3D dimensions, const std::vector<float>& data);
	//ファイルから準備してある3次元テクスチャーを読み込む
	Texture3D(std::string filename);
	~Texture3D();

	ID3D11ShaderResourceView* getShaderResourceView() { return m_shader_res_view; }

	//txtファイルに現在のデーターを出力する
	void outputFile();

private:
	//txtファイルからデーターを読み込む
	void loadFile(std::string filename);

private:
	ID3D11ShaderResourceView* m_shader_res_view = nullptr;
	std::vector<float> m_data;

	int m_height;
	int m_width;
	int m_depth;
private:
	friend class DeviceContext;
};

