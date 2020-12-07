#include "StaticMesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "DeviceContext.h"

#include <locale>
#include <codecvt>

#include "GraphicsEngine.h"
#include "VertexMesh.h"
#include "ConstantBufferSystem.h"

StaticMesh::StaticMesh(const wchar_t* full_path) : Mesh(full_path)
{
	StaticMesh(full_path, false);
}

StaticMesh::StaticMesh(const wchar_t* full_path, bool is_flipped) : Mesh(full_path)
{
	
	std::vector<VertexMesh> vertices;
	std::vector<u_int> indices;

	u_int current_index = 0;

	std::vector<Vector3D> positions;
	std::vector<Vector3D> normals;
	std::vector<Vector2D> texcoords;

	std::wifstream fin(m_full_path);  
	_ASSERT_EXPR(fin, L"'OBJ file not found.");

	wchar_t command[256];
	std::wstring mtl_path = {};

	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"mtllib"))	//mtlib load
		{
			wchar_t mtlfile[256] = {};
			fin >> mtlfile;

			UINT t_length = wcslen(mtlfile);
			
			mtl_path = createFilepath(m_full_path, m_full_path.length(), mtlfile, t_length);
		}
		else if (0 == wcscmp(command, L"usemtl"))	//material subset
		{
			Subset_Obj temp_sub = {};
			temp_sub.m_index_start = indices.size();
			fin >> temp_sub.m_mtl;
			m_subs.push_back(temp_sub);
		}
		else if (0 == wcscmp(command, L"v"))	//position vectors
		{
			float x, y, z;
			fin >> x >> y >> z;
			positions.push_back(Vector3D(x, y, z));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vt")) //texcoords
		{
			float u, v, w;
			fin >> u >> v;

			if (is_flipped) v = 1.0f - v;
			texcoords.push_back(Vector2D(u, v));
		}
		else if (0 == wcscmp(command, L"vn")) //normal vectors
		{
			FLOAT i, j, k;
			fin >> i >> j >> k;
			normals.push_back(Vector3D(i, j, k));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"f")) 
		{
			for (u_int i = 0; i < 3; i++)
			{
				VertexMesh vertex;
				u_int v, vt, vn;

				fin >> v;
				vertex.m_position = positions[v - 1];
				if (L'/' == fin.peek())
				{
					fin.ignore();
					if (L'/' != fin.peek())
					{
						fin >> vt;
						vertex.m_texcoord = texcoords[vt - 1];
					}
					if (L'/' == fin.peek())
					{
						fin.ignore();
						fin >> vn;
						vertex.m_normal = normals[vn - 1];
					}
				}
				vertices.push_back(vertex);
				indices.push_back(current_index++);
			}
			fin.ignore(1024, L'\n');
		}
		else
		{
			fin.ignore(1024, L'\n');
		}
	}
	fin.close();

	loadMtlFile(mtl_path);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&vertices[0], sizeof(VertexMesh),
		(UINT)vertices.size(), shader_byte_code, (UINT)size_shader);
	m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());
}

StaticMesh::~StaticMesh()
{
}

void StaticMesh::renderMesh(float elapsed_time/*dummy value*/, Vector3D scale, Vector3D position, Vector3D rotation, int shader, bool is_textured, float animation_speed)
{
	//set the mesh's buffers into the device context for rendering
	//set the vertices which will be drawn
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);
	//set the index for the vertices which will be drawn
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_index_buffer);

	//draw the vertices
	for (int i = 0; i < m_subs.size(); i++)
	{
		int mat_id = 0;
		//find the material for this subset
		for (mat_id; mat_id < m_mats.size(); mat_id++) 
			if (m_mats[mat_id].m_name == m_subs[i].m_mtl) break;

		//set the texture for the current subset
		if (m_mats[mat_id].m_diffuse)
		{
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_mats[mat_id].m_diffuse);
			GraphicsEngine::get()->getShaderManager()->setPixelShader(PS::ONE_TEX);
		}
		else GraphicsEngine::get()->getShaderManager()->setPixelShader(PS::NO_TEX);

		//update the light properties buffer for the current subset
		GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetObjectLightPropertyBuffer(m_mats[mat_id]);

		UINT index_count;
		if (i + 1 < m_subs.size()) index_count = m_subs[i + 1].m_index_start - m_subs[i].m_index_start;
		else index_count = m_index_buffer->getSizeIndexList() - m_subs[i].m_index_start;

		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(index_count, 0, m_subs[i].m_index_start);
	}
}

std::wstring StaticMesh::createFilepath(std::wstring orig_path, UINT size_orig, const wchar_t* target_file, UINT size_target)
{
	int path = 0;
	for (UINT i = 0; i < size_orig; i++)
	{
		if (orig_path[i] == '\\') path = i;
	}
	std::wstring target_filepath = {};
	
	for (UINT i = 0; i <= path + size_target; i++)
	{
		if (i <= path) target_filepath += orig_path[i];
		else target_filepath += target_file[i - path - 1];
	}

	return target_filepath;
}

void StaticMesh::loadMtlFile(std::wstring full_path)
{
	std::wifstream fin(full_path);
	_ASSERT_EXPR(fin, L"'MTL file not found.");


	//this value is set to -1 so that when fin finds the first material, 
	//num_mats increments to 0 and points to the first material in the vector.
	int num_mats = -1; 
	wchar_t command[256];

	while (fin)
	{
		fin >> command;

		if (0 == wcscmp(command, L"newmtl"))	//new material
		{
			std::wstring str = {};
			fin >> str;

			bool is_new = true;
			for (int i = 0; i < m_mats.size(); i++)
			{
				if (m_mats[i].m_name == str)
				{
					is_new = false;
					break;
				}
			}
			if (is_new)
			{
				num_mats++;
				Material_Obj m;
				m.m_name = str;
				m_mats.push_back(m);
			}
		}
		else if (0 == wcscmp(command, L"Ns"))	
		{
			fin >> m_mats[num_mats].m_shininess;
		}
		else if (0 == wcscmp(command, L"Ka"))
		{
			fin >> m_mats[num_mats].m_ambient_color.m_x >> m_mats[num_mats].m_ambient_color.m_y >> m_mats[num_mats].m_ambient_color.m_z;
		}
		else if (0 == wcscmp(command, L"Kd"))
		{
			fin >> m_mats[num_mats].m_diffuse_color.m_x >> m_mats[num_mats].m_diffuse_color.m_y >> m_mats[num_mats].m_diffuse_color.m_z;
		}
		else if (0 == wcscmp(command, L"Ks"))
		{
			fin >> m_mats[num_mats].m_specular_color.m_x >> m_mats[num_mats].m_specular_color.m_y >> m_mats[num_mats].m_specular_color.m_z;
		}
		else if (0 == wcscmp(command, L"d"))
		{
			fin >> m_mats[num_mats].m_transparency;
		}
		//else if (0 == wcscmp(command, L"Tf"))
		//{
		//	fin >> m_mats[num_mats].m_Tf;
		//}
		else if (0 == wcscmp(command, L"map_Ka"))	//diffuse texture
		{
			wchar_t ambient_tex[256] = {};
			fin >> ambient_tex;
			UINT t_length = wcslen(ambient_tex);

			std::wstring ambient_path = createFilepath(m_full_path, m_full_path.length(), ambient_tex, t_length);
			const wchar_t* ambient_path_wchar = ambient_path.c_str();
			
			m_mats[num_mats].m_ambient = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(ambient_path_wchar);
		}
		else if (0 == wcscmp(command, L"map_Kd"))	//diffuse texture
		{
			wchar_t diffuse_tex[256] = {};
			fin >> diffuse_tex;
			UINT t_length = wcslen(diffuse_tex);

			std::wstring diffuse_path = createFilepath(m_full_path, m_full_path.length(), diffuse_tex, t_length);
			const wchar_t* diffuse_path_wchar = diffuse_path.c_str();

			m_mats[num_mats].m_diffuse = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(diffuse_path_wchar);
		}
		else if (0 == wcscmp(command, L"map_Ks"))	//diffuse texture
		{
			wchar_t specular_tex[256] = {};
			fin >> specular_tex;
			UINT t_length = wcslen(specular_tex);

			std::wstring specular_path = createFilepath(m_full_path, m_full_path.length(), specular_tex, t_length);
			const wchar_t* specular_path_wchar = specular_path.c_str();

			m_mats[num_mats].m_specular = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(specular_path_wchar);
		}
		else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump"))	//diffuse texture
		{
			wchar_t bump_tex[256] = {};
			fin >> bump_tex;
			UINT t_length = wcslen(bump_tex);

			std::wstring bump_path = createFilepath(m_full_path, m_full_path.length(), bump_tex, t_length);
			const wchar_t* bump_path_wchar = bump_path.c_str();

			m_mats[num_mats].m_specular = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(bump_path_wchar);
		}
		else
		{
			fin.ignore(1024, L'\n');
		}
	}
	fin.close();
}
