#include "PrimitiveGenerator.h"
#include "VertexMesh.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include "Cube.h"

PrimitiveGenerator* PrimitiveGenerator::instance = nullptr;

PrimitiveGenerator::PrimitiveGenerator()
{
	bool result;

	result = initPrimitiveCube();
	if (!result) assert(0 && "Error occured when initializing the primitive cube buffers.");
}

CubePtr PrimitiveGenerator::createCube(const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Vector3D scale, Vector3D position, Vector3D rotation, Material_Obj* mat)
{
	//continue here
	CubePtr output = std::shared_ptr<Cube>(new Cube(m_cube_vb, m_cube_ib));
	if (diffuse_tex != nullptr) output->loadDiffuseTex(diffuse_tex);
	if (normal_tex != nullptr) output->loadNormalTex(normal_tex);
	if (roughness_tex != nullptr) output->loadRoughnessTex(roughness_tex);

	output->setScale(scale);
	output->setPosition(position);
	output->setRotation(rotation);
	if (mat != nullptr) output->setMaterial(*mat);

	return output;
}

bool PrimitiveGenerator::loadTexture(std::wstring file, std::string name)
{
	std::wstring full_file = L"..\\Assets\\Textures\\PrimitiveTextures\\" + file;
	const wchar_t* tempwchar = full_file.c_str();
	TexturePtr temp = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(tempwchar);
	if (temp == nullptr) return false;

	m_textures.push_back(temp);
	m_tex_names.push_back(name);
	m_tex_files.push_back(file);
	return true;
}

TexturePtr PrimitiveGenerator::findTexture(std::string name)
{
	for (int i = 0; i < m_textures.size(); i++)
	{
		if (m_tex_names[i] == name)
		{
			return m_textures[i];
		}
	}

	return nullptr;
}

void PrimitiveGenerator::clearTextures()
{
	m_textures.clear();
	m_tex_files.clear();
	m_tex_names.clear();
}

bool PrimitiveGenerator::initPrimitiveCube()
{

	Vector3D position_list[] =
	{
		{ Vector3D(-0.5f,-0.5f,-0.5f)},
		{ Vector3D(-0.5f,0.5f,-0.5f) },
		{ Vector3D(0.5f,0.5f,-0.5f) },
		{ Vector3D(0.5f,-0.5f,-0.5f)},

		//BACK FACE
		{ Vector3D(0.5f,-0.5f,0.5f) },
		{ Vector3D(0.5f,0.5f,0.5f) },
		{ Vector3D(-0.5f,0.5f,0.5f)},
		{ Vector3D(-0.5f,-0.5f,0.5f) }
	};

	Vector2D texcoord_list[] =
	{
		{ Vector2D(0.0f,0.0f) },
		{ Vector2D(0.0f,1.0f) },
		{ Vector2D(1.0f,0.0f) },
		{ Vector2D(1.0f,1.0f) }
	};

	Vector3D normal_list[] =
	{
		{ Vector3D(0, 0, -1) },
		{ Vector3D(0, 0, 1) },
		{ Vector3D(0, 1, 0) },
		{ Vector3D(0, -1, 0) },
		{ Vector3D(1, 0, 0) },
		{ Vector3D(-1, 0, 0) },
	};

	VertexMesh vertex_list[] =
	{
		{ position_list[0],texcoord_list[1], normal_list[0] },
		{ position_list[1],texcoord_list[0], normal_list[0] },
		{ position_list[2],texcoord_list[2], normal_list[0] },
		{ position_list[3],texcoord_list[3], normal_list[0] },

		{ position_list[4],texcoord_list[1], normal_list[1] },
		{ position_list[5],texcoord_list[0], normal_list[1] },
		{ position_list[6],texcoord_list[2], normal_list[1] },
		{ position_list[7],texcoord_list[3], normal_list[1] },

		{ position_list[1],texcoord_list[1], normal_list[2] },
		{ position_list[6],texcoord_list[0], normal_list[2] },
		{ position_list[5],texcoord_list[2], normal_list[2] },
		{ position_list[2],texcoord_list[3], normal_list[2] },

		{ position_list[7],texcoord_list[1], normal_list[3] },
		{ position_list[0],texcoord_list[0], normal_list[3] },
		{ position_list[3],texcoord_list[2], normal_list[3] },
		{ position_list[4],texcoord_list[3], normal_list[3] },

		{ position_list[3],texcoord_list[1], normal_list[4] },
		{ position_list[2],texcoord_list[0], normal_list[4] },
		{ position_list[5],texcoord_list[2], normal_list[4] },
		{ position_list[4],texcoord_list[3], normal_list[4] },

		{ position_list[7],texcoord_list[1], normal_list[5] },
		{ position_list[6],texcoord_list[0], normal_list[5] },
		{ position_list[1],texcoord_list[2], normal_list[5] },
		{ position_list[0],texcoord_list[3], normal_list[5] }
	};

	UINT size_vertex_list = ARRAYSIZE(vertex_list);

	unsigned int index_list[] =
	{
		//FRONT SIDE
		0,1,2,  
		2,3,0,  
		//BACK SIDE
		4,5,6,
		6,7,4,
		//TOP SIDE
		8,9,10,
		10,11,8,
		//BOTTOM SIDE
		12,13,14,
		14,15,12,
		//RIGHT SIDE
		16,17,18,
		18,19,16,
		//LEFT SIDE
		20,21,22,
		22,23,20
	};
	UINT size_index_list = ARRAYSIZE(index_list);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

	m_cube_vb = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&vertex_list[0], sizeof(VertexMesh),
		size_vertex_list, shader_byte_code, (UINT)size_shader);
	m_cube_ib = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&index_list[0], size_index_list);


	if (m_cube_vb == nullptr || m_cube_ib == nullptr) return false;
	return true;
}
