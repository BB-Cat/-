#include "Cube.h"
#include "VertexMesh.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include "ConstantBufferSystem.h"
#include "IndexBuffer.h"
#include "PrimitiveGenerator.h"

Cube::Cube(VertexBufferPtr vertexes, IndexBufferPtr indexes)
{
	m_vertex_buffer = vertexes;
	m_index_buffer = indexes;

	m_shader = Shaders::LAMBERT;

	//default cube material values
	m_mat.m_diffuse_color = Vector3D(0.6f, 0.7f, 0.6f);
	m_mat.m_transparency = 1.0f;
	m_mat.m_diffuse_color = Vector3D(0.7f, 0.7f, 0.7f);
	m_mat.m_metallicAmount = 0.445f;
	m_mat.m_shininess = 3;
	m_mat.m_specular_color = Vector3D(0.5, 0.2, 0.5);
	m_mat.m_rim_color = Vector3D(0.4f, 0.7f, 0.7f);
	m_mat.m_rim_color.m_w = 1.0f;
	m_mat.m_rim_power = 4.4f;

	m_scale = Vector3D(1, 1, 1);
	m_pos = Vector3D(0, 0, 0);
	m_rot = Vector3D(0, 0, 0);

	HRESULT hr;

	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0;
	rsDesc.SlopeScaledDepthBias = 0;
	rsDesc.DepthClipEnable = false;
	rsDesc.ScissorEnable = false;
	rsDesc.MultisampleEnable = false;
	rsDesc.AntialiasedLineEnable = false;
	hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rsDesc, &m_solid_rast);
	if (FAILED(hr))	assert(0 && "Error loading the solid fill rasterizer");

	m_mesh_world.setIdentity();
}

Cube::~Cube()
{
	if (m_solid_rast) m_solid_rast->Release();
}

void Cube::render(int shader, bool is_textured)
{
	render(m_scale, m_pos, m_rot, shader, is_textured);
}

void Cube::render(Vector3D scale, Vector3D position, Vector3D rotation, int shader, bool is_textured)
{
	//set the vertices which will be drawn
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);
	//set the index for the vertices which will be drawn
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_index_buffer);
	//set the shader
	if (shader >= 0) GraphicsEngine::get()->getShaderManager()->setPipeline(shader);
	else GraphicsEngine::get()->getShaderManager()->setPipeline(m_shader);

	if (is_textured)
	{
		//set textures for the pixel shader
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseNormalGlossTexPS
		(
			m_diffuse,
			m_normal,
			m_roughness
		);
	}
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetObjectLightPropertyBuffer(m_mat);

	//make a dummy array of identity bone transforms so we can use skinned mesh shaders
	Matrix4x4	bone_transforms[MAXBONES];
	for (size_t k = 0; k < MAXBONES; k++) bone_transforms[k].setIdentity();

	Matrix4x4 temp = applyTransformations(m_mesh_world, scale, rotation, position);
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetTransformationBuffer(temp, bone_transforms);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList
	(m_index_buffer->getSizeIndexList(), 0, 0);
}

Matrix4x4 Cube::applyTransformations(const Matrix4x4& global, Vector3D scale, Vector3D rot, Vector3D translate)
{
	Matrix4x4 out = global;
	Matrix4x4 temp;
	temp.setIdentity();

	//Scale
	temp.setScale(scale);
	out *= temp;

	//Rotation
	temp.setIdentity();
	temp.setRotationZ(rot.m_z);
	out *= temp;
	temp.setIdentity();
	temp.setRotationX(rot.m_x);
	out *= temp;
	temp.setIdentity();
	temp.setRotationY(rot.m_y);
	out *= temp;

	//Translation
	out.setTranslation(translate);

	return out;

}

bool Cube::loadDiffuseTex(const wchar_t* file)
{
	TexturePtr temp = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(file);
	if (temp == nullptr) return false;

	m_diffuse = temp;
	return true;
}

bool Cube::loadNormalTex(const wchar_t* file)
{
	TexturePtr temp = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(file);
	if (temp == nullptr) return false;

	m_normal = temp;
	return true;
}

bool Cube::loadRoughnessTex(const wchar_t* file)
{
	TexturePtr temp = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(file);
	if (temp == nullptr) return false;

	m_roughness = temp;
	return true;
}

bool Cube::loadDiffuseTex(TexturePtr tex, std::string name)
{
	m_diffuse = tex;
	m_diffuse_name = name;
	return true;
}

bool Cube::loadNormalTex(TexturePtr tex, std::string name)
{
	m_normal = tex;
	m_normal_name = name;
	return true;
}

bool Cube::loadRoughnessTex(TexturePtr tex, std::string name)
{
	m_roughness = tex;
	m_roughness_name = name;
	return true;
}

bool Cube::fetchDiffuseTex(std::string name)
{
	m_diffuse = PrimitiveGenerator::get()->findTexture(name);
	if(m_diffuse == nullptr) return false;

	m_diffuse_name = name;
	return true;
}

bool Cube::fetchNormalTex(std::string name)
{
	m_normal = PrimitiveGenerator::get()->findTexture(name);
	if (m_diffuse == nullptr) return false;

	m_normal_name = name;
	return true;
}

bool Cube::fetchRoughnessTex(std::string name)
{
	m_roughness = PrimitiveGenerator::get()->findTexture(name);
	if (m_diffuse == nullptr) return false;

	m_roughness_name = name;
	return true;
}
