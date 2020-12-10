#include "PrimitiveGenerator.h"
#include "VertexMesh.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include "Primitive.h"

#include <vector>
//#define _USE_MATH_DEFINES
//#include <math.h>

PrimitiveGenerator* PrimitiveGenerator::instance = nullptr;

PrimitiveGenerator::PrimitiveGenerator()
{
	bool result;

	result = initPrimitiveCube();
	if (!result) assert(0 && "Error occured when initializing the primitive cube buffers.");

	result = initPrimitiveSphere();
	if (!result) assert(0 && "Error occured when initializing the primitive sphere buffers.");

	result = initPrimitiveCapsule();
	if(!result) assert(0 && "Error occured when initializing the primitive capsule buffers.");
}

PrimitivePtr PrimitiveGenerator::createUnitCube(const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat)
{
	PrimitivePtr output = std::shared_ptr<Primitive>(new Primitive(m_cube_vb, m_cube_ib));
	initTextures(output, diffuse_tex, normal_tex, roughness_tex, mat);

	return output;
}

PrimitivePtr PrimitiveGenerator::createUnitSphere(const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat)
{
	PrimitivePtr output = std::shared_ptr<Primitive>(new Primitive(m_sphere_vb, m_sphere_ib));
	initTextures(output, diffuse_tex, normal_tex, roughness_tex, mat);

	return output;
}

PrimitivePtr PrimitiveGenerator::createUnitCapsule(const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat)
{
	PrimitivePtr output = std::shared_ptr<Primitive>(new Primitive(m_capsule_vb, m_capsule_ib));
	initTextures(output, diffuse_tex, normal_tex, roughness_tex, mat);

	return output;
}

PrimitivePtr PrimitiveGenerator::createCustomSphere(float radius, int stacks, int slices, const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat)
{
	VertexBufferPtr vb;
	IndexBufferPtr ib;
	generateSphereGeometry(radius, stacks, slices, vb, ib);
	PrimitivePtr output = std::shared_ptr<Primitive>(new Primitive(vb, ib));
	initTextures(output, diffuse_tex, normal_tex, roughness_tex, mat);

	return output;
}

PrimitivePtr PrimitiveGenerator::createCustomCapsule(float radius, float core_height, int stacks, int slices, const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat)
{
	VertexBufferPtr vb;
	IndexBufferPtr ib;
	generateCapsuleGeometry(radius, core_height, stacks, slices, vb, ib);
	PrimitivePtr output = std::shared_ptr<Primitive>(new Primitive(vb, ib));
	initTextures(output, diffuse_tex, normal_tex, roughness_tex, mat);

	return output;
}

void PrimitiveGenerator::initTextures(PrimitivePtr& output, const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat)
{
	if (diffuse_tex != nullptr) output->loadDiffuseTex(diffuse_tex);
	if (normal_tex != nullptr) output->loadNormalTex(normal_tex);
	if (roughness_tex != nullptr) output->loadRoughnessTex(roughness_tex);
	if (mat != nullptr) output->setMaterial(*mat);
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

bool PrimitiveGenerator::generateSphereGeometry(float radius, int stacks, int slices, VertexBufferPtr& vb, IndexBufferPtr& ib)
{
	int stack_count = stacks;
	int slice_count = slices;

	std::vector<VertexMesh> v_list;
	v_list.clear();
	std::vector<unsigned int> i_list;
	i_list.clear();

	float const R = 1.0 / (float)(stack_count - 1);
	float const S = 1.0 / (float)(slice_count - 1);
	int r, s;
	VertexMesh temp;

	/* there is some strange problem including and defining M_PI so I am defining them manually here */
	float PI = 3.14159265358979323846f;
	float PI_2 = 1.57079632679489661923f;

	for (r = 0; r < stack_count; r++)
	{
		for (s = 0; s < slice_count; s++)
		{
			float y = sin(-PI_2 + PI * r * R);
			float x = cos(2 * PI * s * S) * sin(PI * r * R);
			float z = sin(2 * PI * s * S) * sin(PI * r * R);

			temp.m_texcoord = Vec2(s * S, r * R);

			temp.m_position = Vec3(x * radius, y * radius, z * radius);

			temp.m_normal = Vec3(x, y, z);

			v_list.push_back(temp);
		}
	}

	//build the indexes for this stack
	for (r = 0; r < stack_count; r++)
	{
		for (s = 0; s < slice_count; s++)
		{
			i_list.push_back(r * slice_count + s);

			i_list.push_back((r + 1) * slice_count + (s + 1));

			i_list.push_back(r * slice_count + (s + 1));

			i_list.push_back(r * slice_count + s);

			i_list.push_back((r + 1) * slice_count + s);

			i_list.push_back((r + 1) * slice_count + (s + 1));
		}
	}

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

	vb = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&v_list[0], sizeof(VertexMesh),
		v_list.size(), shader_byte_code, (UINT)size_shader);
	ib = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&i_list[0], i_list.size());

	if (vb == nullptr || ib == nullptr) return false;
	return true;
}

bool PrimitiveGenerator::generateCapsuleGeometry(float radius, float core_height, int stacks, int slices, VertexBufferPtr& vb, IndexBufferPtr& ib)
{
	int stack_count = stacks;
	int slice_count = slices;

	std::vector<VertexMesh> v_list;
	v_list.clear();
	std::vector<unsigned int> i_list;
	i_list.clear();

	float const R = 1.0 / (float)(stack_count - 1);
	float const S = 1.0 / (float)(slice_count - 1);
	int r, s;
	float c;
	VertexMesh temp;

	/* there is some strange problem including and defining M_PI so I am defining them manually here */
	float PI = 3.14159265358979323846f;
	float PI_2 = 1.57079632679489661923f;

	for (r = 0; r < stack_count; r++)
	{
		for (s = 0; s < slice_count; s++)
		{
			float y = sin(-PI_2 + PI * r * R);
			float x = cos(2 * PI * s * S) * sin(PI * r * R);
			float z = sin(2 * PI * s * S) * sin(PI * r * R);

			temp.m_texcoord = Vec2(s * S, r * R);

			//if (r < stack_count / 2) c = -(core_height / 2.0f);
			//else c = core_height / 2.0f;

			c = r < stack_count / 2 ? -(core_height / 2.0f) : core_height / 2.0f;

			temp.m_position = Vec3(x * radius, y * radius + c, z * radius);

			temp.m_normal = Vec3(x, y, z);

			v_list.push_back(temp);
		}
	}

	//build the indexes for this stack
	for (r = 0; r < stack_count; r++)
	{
		for (s = 0; s < slice_count; s++)
		{
			i_list.push_back(r * slice_count + s);

			i_list.push_back((r + 1) * slice_count + (s + 1));

			i_list.push_back(r * slice_count + (s + 1));

			i_list.push_back(r * slice_count + s);

			i_list.push_back((r + 1) * slice_count + s);

			i_list.push_back((r + 1) * slice_count + (s + 1));
		}
	}

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

	vb = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&v_list[0], sizeof(VertexMesh),
		v_list.size(), shader_byte_code, (UINT)size_shader);
	ib = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&i_list[0], i_list.size());

	if (vb == nullptr || ib == nullptr) return false;
	return true;
}

bool PrimitiveGenerator::initPrimitiveCube()
{

	Vec3 position_list[] =
	{
		{ Vec3(-0.5f,-0.5f,-0.5f)},
		{ Vec3(-0.5f,0.5f,-0.5f) },
		{ Vec3(0.5f,0.5f,-0.5f) },
		{ Vec3(0.5f,-0.5f,-0.5f)},

		//BACK FACE
		{ Vec3(0.5f,-0.5f,0.5f) },
		{ Vec3(0.5f,0.5f,0.5f) },
		{ Vec3(-0.5f,0.5f,0.5f)},
		{ Vec3(-0.5f,-0.5f,0.5f) }
	};

	Vec2 texcoord_list[] =
	{
		{ Vec2(0.0f,0.0f) },
		{ Vec2(0.0f,1.0f) },
		{ Vec2(1.0f,0.0f) },
		{ Vec2(1.0f,1.0f) }
	};

	Vec3 normal_list[] =
	{
		{ Vec3(0, 0, -1) },
		{ Vec3(0, 0, 1) },
		{ Vec3(0, 1, 0) },
		{ Vec3(0, -1, 0) },
		{ Vec3(1, 0, 0) },
		{ Vec3(-1, 0, 0) },
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

bool PrimitiveGenerator::initPrimitiveSphere()
{
	int stack_count = 30;
	int slice_count = 30;
	float radius = 0.5f;

	return generateSphereGeometry(radius, stack_count, slice_count, m_sphere_vb, m_sphere_ib);
	//std::vector<VertexMesh> v_list;
	//v_list.clear();
	//std::vector<unsigned int> i_list;
	//i_list.clear();

	//float const R = 1.0 / (float)(stack_count - 1);
	//float const S = 1.0 / (float)(slice_count - 1);
	//int r, s;
	//VertexMesh temp;

	///* there is some strange problem including and defining M_PI so I am defining them manually here */
	//float PI = 3.14159265358979323846f;
	//float PI_2 = 1.57079632679489661923f;

	//for (r = 0; r < stack_count; r++) 
	//{
	//	for (s = 0; s < slice_count; s++) 
	//	{
	//		float y = sin(-PI_2 + PI * r * R);
	//		float x = cos(2 * PI * s * S) * sin(PI * r * R);
	//		float z = sin(2 * PI * s * S) * sin(PI * r * R);

	//		temp.m_texcoord = Vec2(s * S, r * R);

	//		temp.m_position = Vec3(x * radius, y * radius, z * radius);

	//		temp.m_normal = Vec3(x, y, z);

	//		v_list.push_back(temp);
	//	}
	//}

	////build the indexes for this stack
	//for (r = 0; r < stack_count; r++)
	//{
	//	for (s = 0; s < slice_count; s++)
	//	{
	//		i_list.push_back(r * slice_count + s);

	//		i_list.push_back((r + 1) * slice_count + (s + 1));

	//		i_list.push_back(r * slice_count + (s + 1));

	//		i_list.push_back(r * slice_count + s);

	//		i_list.push_back((r + 1) * slice_count + s);

	//		i_list.push_back((r + 1) * slice_count + (s + 1));
	//	}
	//}

	//void* shader_byte_code = nullptr;
	//size_t size_shader = 0;
	//GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

	//m_sphere_vb = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&v_list[0], sizeof(VertexMesh),
	//	v_list.size(), shader_byte_code, (UINT)size_shader);
	//m_sphere_ib = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&i_list[0], i_list.size());

	//if (m_sphere_vb == nullptr || m_sphere_ib == nullptr) return false;
	//return true;
}

bool PrimitiveGenerator::initPrimitiveCapsule()
{
	int stack_count = 30;
	int slice_count = 30;
	float radius = 0.25f;
	float core_height = 0.5f;

	return generateCapsuleGeometry(radius, core_height, stack_count, slice_count, m_capsule_vb, m_capsule_ib);

	//std::vector<VertexMesh> v_list;
	//v_list.clear();
	//std::vector<unsigned int> i_list;
	//i_list.clear();

	//float const R = 1.0 / (float)(stack_count - 1);
	//float const S = 1.0 / (float)(slice_count - 1);
	//int r, s;
	//float c;
	//VertexMesh temp;

	///* there is some strange problem including and defining M_PI so I am defining them manually here */
	//float PI = 3.14159265358979323846f;
	//float PI_2 = 1.57079632679489661923f;

	//for (r = 0; r < stack_count; r++)
	//{
	//	for (s = 0; s < slice_count; s++)
	//	{
	//		float y = sin(-PI_2 + PI * r * R);
	//		float x = cos(2 * PI * s * S) * sin(PI * r * R);
	//		float z = sin(2 * PI * s * S) * sin(PI * r * R);

	//		temp.m_texcoord = Vec2(s * S, r * R);

	//		//if (r < stack_count / 2) c = -(core_height / 2.0f);
	//		//else c = core_height / 2.0f;

	//		c = r < stack_count / 2 ? -(core_height / 2.0f) : core_height / 2.0f;

	//		temp.m_position = Vec3(x * radius, y * radius + c, z * radius);

	//		temp.m_normal = Vec3(x, y, z);

	//		v_list.push_back(temp);
	//	}
	//}

	////build the indexes for this stack
	//for (r = 0; r < stack_count; r++)
	//{
	//	for (s = 0; s < slice_count; s++)
	//	{
	//		i_list.push_back(r * slice_count + s);

	//		i_list.push_back((r + 1) * slice_count + (s + 1));

	//		i_list.push_back(r * slice_count + (s + 1));

	//		i_list.push_back(r * slice_count + s);

	//		i_list.push_back((r + 1) * slice_count + s);

	//		i_list.push_back((r + 1) * slice_count + (s + 1));
	//	}
	//}

	//void* shader_byte_code = nullptr;
	//size_t size_shader = 0;
	//GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

	//m_capsule_vb = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&v_list[0], sizeof(VertexMesh),
	//	v_list.size(), shader_byte_code, (UINT)size_shader);
	//m_capsule_ib = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&i_list[0], i_list.size());


}
