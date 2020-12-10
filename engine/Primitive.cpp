#include "Primitive.h"
#include "VertexMesh.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include "ConstantBufferSystem.h"
#include "IndexBuffer.h"
#include "PrimitiveGenerator.h"
#include "Colliders.h"

Primitive::Primitive(VertexBufferPtr vertexes, IndexBufferPtr indexes)
{
	m_vertex_buffer = vertexes;
	m_index_buffer = indexes;

	//default cube material values
	m_mat.m_diffuse_color = Vec3(0.6f, 0.7f, 0.6f);
	m_mat.m_transparency = 1.0f;
	m_mat.m_diffuse_color = Vec3(0.7f, 0.7f, 0.7f);
	m_mat.m_metallic = 0.445f;
	m_mat.m_shininess = 3;
	m_mat.m_specular_color = Vec3(0.5, 0.2, 0.5);
	m_mat.m_rim_color = Vec3(0.4f, 0.7f, 0.7f);
	m_mat.m_rim_color.m_w = 1.0f;
	m_mat.m_rim_power = 4.4f;

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

Primitive::~Primitive()
{
	if (m_solid_rast) m_solid_rast->Release();
}

void Primitive::render(Vec3 scale, Vec3 position, Vec3 rotation, int shader, bool is_textured)
{
	//set the vertices which will be drawn
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);
	//set the index for the vertices which will be drawn
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_index_buffer);
	//set the shader
	GraphicsEngine::get()->getShaderManager()->setPipeline(shader);
	//set the raster state
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_solid_rast);

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

Matrix4x4 Primitive::applyTransformations(const Matrix4x4& global, Vec3 scale, Vec3 rot, Vec3 translate)
{
	Matrix4x4 out = global;
	Matrix4x4 temp;
	temp.setIdentity();

	//Scale
	temp.setScale(scale);
	out *= temp;

	//Rotation
	temp.setIdentity();
	temp.setRotationZ(rot.z);
	out *= temp;
	temp.setIdentity();
	temp.setRotationX(rot.x);
	out *= temp;
	temp.setIdentity();
	temp.setRotationY(rot.y);
	out *= temp;

	//Translation
	out.setTranslation(translate);

	return out;

}

bool Primitive::loadDiffuseTex(const wchar_t* file)
{
	TexturePtr temp = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(file);
	if (temp == nullptr) return false;

	m_diffuse = temp;
	return true;
}

bool Primitive::loadNormalTex(const wchar_t* file)
{
	TexturePtr temp = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(file);
	if (temp == nullptr) return false;

	m_normal = temp;
	return true;
}

bool Primitive::loadRoughnessTex(const wchar_t* file)
{
	TexturePtr temp = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(file);
	if (temp == nullptr) return false;

	m_roughness = temp;
	return true;
}

bool Primitive::loadDiffuseTex(TexturePtr tex, std::string name)
{
	m_diffuse = tex;
	m_diffuse_name = name;
	return true;
}

bool Primitive::loadNormalTex(TexturePtr tex, std::string name)
{
	m_normal = tex;
	m_normal_name = name;
	return true;
}

bool Primitive::loadRoughnessTex(TexturePtr tex, std::string name)
{
	m_roughness = tex;
	m_roughness_name = name;
	return true;
}

bool Primitive::fetchDiffuseTex(std::string name)
{
	m_diffuse = PrimitiveGenerator::get()->findTexture(name);
	if (m_diffuse == nullptr) return false;

	m_diffuse_name = name;
	return true;
}

bool Primitive::fetchNormalTex(std::string name)
{
	m_normal = PrimitiveGenerator::get()->findTexture(name);
	if (m_diffuse == nullptr) return false;

	m_normal_name = name;
	return true;
}

bool Primitive::fetchRoughnessTex(std::string name)
{
	m_roughness = PrimitiveGenerator::get()->findTexture(name);
	if (m_diffuse == nullptr) return false;

	m_roughness_name = name;
	return true;
}

void Primitive::setColor(Vec3 col)
{
	m_mat.m_diffuse_color = col;
}

void Primitive::setTransparency(float t)
{
	m_mat.m_transparency = t;
}
