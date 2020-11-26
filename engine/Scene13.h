#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "ConstantBufferFormats.h"
#include "PrimitiveGenerator.h"

//#include "Subset.h"
#include "VertexMesh.h"

class Scene13 : public Scene
{
private:
    bool m_first_time = false;
    bool m_show_wire = false;
    int m_active_shader = Shaders::TERRAIN_TEST;
    std::vector<VertexMesh> m_verts;
    TerrainPtr m_terrain[1024];
    TerrainManager* m_tman;
    int m_create_terrain_timer = -1;

    IndexBufferPtr m_hd, m_md, m_ld;
    IndexBufferPtr m_LOD_seam_high;
    IndexBufferPtr m_LOD_seam_mid;
    IndexBufferPtr m_LOD_seam_low;

    IndexBufferPtr m_LOD_midToHigh;
    IndexBufferPtr m_LOD_highToMid;

    IndexBufferPtr m_LOD_midToLow;
    IndexBufferPtr m_LOD_lowToMid;

    ComputeShaderPtr m_compute_terrain;
    ComputeShaderPtr m_compute_noisetex;
    ID3D11ShaderResourceView* m_srv;
    ID3D11RasterizerState* m_rs;
    ID3D11RasterizerState* m_rs2;

    cb_noise m_noise;
    int m_int_vor_octave;
    //int m_int_vor_frequency;
    int m_int_per_octave;
    //int m_int_per_frequency;
    int m_int_ridge_per_octave;
    //int m_int_ridge_per_frequency;

public:
    Scene13(SceneManager*);
    ~Scene13();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

    void makeComputeShaderTexture();
    void runTerrainComputeShader();

    //temp - taken from terrain manager for testing
    void initTerrainBuffers();
    //temp - taken from terrain manager for testing
    void initRasterizers();
    //temp - taken from terrain manager for testing
    void initTextures();

    void initNoiseBuffer();

private:
    TexturePtr m_displace1_1;
    TexturePtr m_tex1;
    TexturePtr m_norm1;
    TexturePtr m_rough1;
    TexturePtr m_ambient_occ1;

    TexturePtr m_displace2_1;
    TexturePtr m_tex2;
    TexturePtr m_norm2;
    TexturePtr m_rough2;
    TexturePtr m_ambient_occ2;

    TexturePtr m_displace3_1;
    TexturePtr m_tex3;
    TexturePtr m_norm3;
    TexturePtr m_rough3;
    TexturePtr m_ambient_occ3;

    TexturePtr m_displace4_1;
    TexturePtr m_tex4;
    TexturePtr m_norm4;
    TexturePtr m_rough4;
    TexturePtr m_ambient_occ4;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};