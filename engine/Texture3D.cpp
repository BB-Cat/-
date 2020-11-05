#include "Texture3D.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"

#include <iostream>
#include <fstream>  

Texture3D::Texture3D(Vector3D dimensions, const std::vector<float>& data)
{
    // Create texture3d
    const UINT cWidth = dimensions.m_x;
    const UINT cHeight = dimensions.m_y;
    const UINT cDepth = dimensions.m_z;
    m_width = cWidth;
    m_height = cHeight;
    m_depth = cDepth;

    const UINT miplevels = 1;

    D3D11_TEXTURE3D_DESC desc;
    desc.Width = cWidth;
    desc.Height = cHeight;
    desc.MipLevels = miplevels;
    desc.Depth = cDepth;

    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    


    const UINT bytesPerPixel = 16;
    UINT sliceSize = cWidth * cHeight * bytesPerPixel;

    D3D11_SUBRESOURCE_DATA initData = { 0 };
    initData.pSysMem = &data[0];
    initData.SysMemPitch = cWidth * bytesPerPixel;
    initData.SysMemSlicePitch = cWidth * cHeight * bytesPerPixel;


    ID3D11Texture3D* tex = nullptr;
    GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture3D(&desc, &initData, &tex);  
    GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(tex, NULL, &m_shader_res_view);

    m_data = data;
}

Texture3D::Texture3D(std::string filename)
{
    loadFile(filename);

    // Create texture3d
    const UINT cWidth = m_width;
    const UINT cHeight = m_height;
    const UINT cDepth = m_depth;

    const UINT miplevels = 1;

    D3D11_TEXTURE3D_DESC desc;
    desc.Width = cWidth;
    desc.Height = cHeight;
    desc.MipLevels = miplevels;
    desc.Depth = cDepth;

    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;


    const UINT bytesPerPixel = 16;
    UINT sliceSize = cWidth * cHeight * bytesPerPixel;

    D3D11_SUBRESOURCE_DATA initData = { 0 };
    initData.pSysMem = &m_data[0];
    initData.SysMemPitch = cWidth * bytesPerPixel;
    initData.SysMemSlicePitch = cWidth * cHeight * bytesPerPixel;

    ID3D11Texture3D* tex = nullptr;
    GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture3D(&desc, &initData, &tex);
    GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(tex, NULL, &m_shader_res_view);
}

Texture3D::~Texture3D()
{
	m_shader_res_view->Release();
}

void Texture3D::outputFile()
{

    std::string folder = "..\\Assets\\3DTextures\\Generated\\";

    std::string size_x, size_y, size_z;
    size_x = std::to_string((int)(m_width));
    size_y = std::to_string((int)(m_height));
    size_z = std::to_string((int)(m_depth));

    //====================================================================================
        //output the chunk data
    //------------------------------------------------------------------------------------
    std::string file = folder + size_x + "_" + size_y + "_" + size_z + "_cloud.txt";
    std::ofstream outfile(file.c_str());

    float val;

    //record the xyz dimensions
    outfile << std::to_string((int)m_width) << " " << std::to_string((int)m_height) << " " << std::to_string((int)m_depth) << " ";

    for (int i = 0; i < m_data.size(); i++)
    {
        val = m_data[i];
        outfile << std::to_string(val) << " ";
    }

    outfile.close();
}

void Texture3D::loadFile(std::string filename)
{
    //std::string file = std::to_string((int)(m_pos.m_x)) + "_" + std::to_string((int)(m_pos.m_y)) + "_chunk.txt";
    std::string file = filename;
    std::ifstream fin;
  
    fin = std::ifstream("..\\Assets\\3DTextures\\" + file);

    _ASSERT_EXPR(fin.is_open(), L"3D Texture File not found!");

    float val;
    //xyz‚Ì‘å‚«‚³‚ð“Ç‚Ýž‚Þ
    fin >> m_width;
    fin >> m_height;
    fin >> m_depth;

    int totalsize = m_width * m_height * m_depth * 4; //4 is the number of values RGBA per voxel

    float value;

    for (int i = 0; i < totalsize; i++)
    {
        fin >> value;
        m_data.push_back(value);
    }
}
