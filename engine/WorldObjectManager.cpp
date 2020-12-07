#include "WorldObjectManager.h"
#include "Texture.h"
#include "WorldObject.h"
#include "VectorToArray.h"
#include "Blend.h"
#include "Primitive.h"
#include "PrefabManager.h"
#include "DeviceContext.h"
#include "ShaderManager.h"
#include "CameraManager.h"
#include "SkinnedMesh.h"

#include <iostream>
#include <fstream>  

WorldObjectManager* WorldObjectManager::instance = nullptr;

WorldObjectManager::WorldObjectManager()
{

}

WorldObjectManager* WorldObjectManager::get()
{
	if (instance == nullptr) instance = new WorldObjectManager();

	return instance;
}

void WorldObjectManager::outputSceneData(std::string filename)
{
	std::string folder = "..\\Assets\\SceneData\\";

	//====================================================================================
		//output the chunk data
	//------------------------------------------------------------------------------------
	std::string file = folder + filename + ".txt";
	std::ofstream outfile(file.c_str());

	std::vector<TexturePtr> temp_tex = PrimitiveGenerator::get()->getTextures();
	std::vector<std::string> temp_texname = PrimitiveGenerator::get()->getTextureNames();
	std::vector<std::wstring> temp_texfile = PrimitiveGenerator::get()->getTextureFiles();

	/* PER SCENE */
	//number of texture files
	outfile << std::to_string(temp_tex.size()) << " ";
	//texture files and names
	char tempchar[1024] = {};
	std::string texfile;
	for (int i = 0; i < temp_tex.size(); i++)
	{
		//convert the texture filename to string from wstring
		wcstombs(tempchar, temp_texfile[i].c_str(), temp_texfile[i].length());
		//convert to a string for output
		texfile = tempchar;
		//output the texture name, then the texture file
		outfile << temp_texname[i] << " " << texfile << " ";
	}


	std::vector<PrefabMesh> temp_prefabs = PrefabManager::get()->getPrefabs();
	std::vector<std::string> temp_prefabname = PrefabManager::get()->getPrefabNames();
	std::vector<std::wstring> temp_prefabfile = PrefabManager::get()->getPrefabFiles();

	//number of prefab meshes
	outfile << std::to_string(temp_prefabs.size()) << " ";
	//mesh files and names
	std::string prefabfile;
	for (int i = 0; i < temp_prefabs.size(); i++)
	{
		//convert the texture filename to string from wstring
		wcstombs(tempchar, temp_prefabfile[i].c_str(), temp_prefabfile[i].length());
		//convert to a string for output
		prefabfile = tempchar;
		//output the texture name, then the texture file
		outfile << temp_prefabname[i] << " " << prefabfile << " ";
	}


	//number of cubes
	int num_cubes = 0;
	std::vector<WorldObjectPtr> cubes;
	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->getObjectType() == ObjectType::Primitive)
		{
			num_cubes++;
			cubes.push_back(m_objects[i]);
		}
	}
	outfile << std::to_string(num_cubes) << " ";

	//number of prefabs
	int num_prefabs = 0;
	std::vector<WorldObjectPtr> prefabs;
	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->getObjectType() == ObjectType::Mesh)
		{
			num_prefabs++;
			prefabs.push_back(m_objects[i]);
		}
	}
	outfile << std::to_string(num_prefabs) << " ";


	Vector4D temp;
	/* PER CUBE */
	for (int i = 0; i < cubes.size(); i++)
	{
		//output the diffuse texname
		outfile << cubes[i]->getPrimitive()->getDiffuseName() << " ";
		//output the normal texname
		outfile << cubes[i]->getPrimitive()->getNormalName() << " ";
		//output the roughness texname
		outfile << cubes[i]->getPrimitive()->getRoughnessName() << " ";
		//output the shader type
		outfile << cubes[i]->getShader() << " ";

		//output the scale
		temp = cubes[i]->getScale();
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";

		//output the position
		temp = cubes[i]->getPosition();
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";

		//output the rotation
		temp = cubes[i]->getRotation();
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";

		/* material data */
		outfile << std::to_string(cubes[i]->getPrimitive()->getMaterial().m_shininess) << " ";
		outfile << std::to_string(cubes[i]->getPrimitive()->getMaterial().m_rim_power) << " ";
		outfile << std::to_string(cubes[i]->getPrimitive()->getMaterial().m_transparency) << " ";

		temp = cubes[i]->getPrimitive()->getMaterial().m_ambient_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		temp = cubes[i]->getPrimitive()->getMaterial().m_diffuse_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		temp = cubes[i]->getPrimitive()->getMaterial().m_specular_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		temp = cubes[i]->getPrimitive()->getMaterial().m_rim_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";
		/* ------------------------------ */

		/* WE ARE ASSUMING ALL CUBES HAVE A CUBE COLLIDER BY NOT ADDING THAT DATA HERE */

	}

	Collider* temp_c = nullptr;
	/* PER PREFAB */
	for (int i = 0; i < prefabs.size(); i++)
	{
		//output the prefab name
		outfile << prefabs[i]->getPrefabName() << " ";
		
		//output the shader type
		outfile << prefabs[i]->getShader() << " ";

		//output the scale
		temp = prefabs[i]->getScale();
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";

		//output the position
		temp = prefabs[i]->getPosition();
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";

		//output the rotation
		temp = prefabs[i]->getRotation();
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";

		/* material data */
		outfile << std::to_string(prefabs[i]->getMaterial().m_shininess) << " ";
		outfile << std::to_string(prefabs[i]->getMaterial().m_rim_power) << " ";
		outfile << std::to_string(prefabs[i]->getMaterial().m_transparency) << " ";

		temp = prefabs[i]->getMaterial().m_ambient_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		temp = prefabs[i]->getMaterial().m_diffuse_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		temp = prefabs[i]->getMaterial().m_specular_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		temp = prefabs[i]->getMaterial().m_rim_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		//collider
		temp_c = prefabs[i]->getCollider();
		if (temp_c != nullptr)
		{
			//has collider
			outfile << "TRUE" << " ";

			//collider type
			outfile << std::to_string(temp_c->getType()) << " ";

			//Vector3D m_bounding_box;
			temp = temp_c->getBoundingBox();
			outfile << std::to_string(temp.m_x) << " ";
			outfile << std::to_string(temp.m_y) << " ";
			outfile << std::to_string(temp.m_z) << " ";

			//Vector3D m_offset;
			temp = temp_c->getOffset();
			outfile << std::to_string(temp.m_x) << " ";
			outfile << std::to_string(temp.m_y) << " ";
			outfile << std::to_string(temp.m_z) << " ";
		}
		else
		{
			//has NO collider
			outfile << "FALSE" << " ";
		}

	}


	outfile.close();
}

bool WorldObjectManager::loadSceneData(std::string filename)
{
	m_objects.clear();
	PrimitiveGenerator::get()->clearTextures();

	std::string file = filename;
	std::ifstream fin;

	fin = std::ifstream("..\\Assets\\SceneData\\" + file);

	//_ASSERT_EXPR(fin.is_open(), L"Scene Data file not found!");
	if (!fin.is_open()) return false;

	std::vector<TexturePtr> temp_tex;
	std::vector<std::string> temp_texname;
	std::vector<std::wstring> temp_texfile;

	int num_tex;
	int num_prefabs;
	std::string tempname = {};
	wchar_t tempwchar[128] = {};
	std::string tempfile = {};

	fin >> num_tex;
	for (int i = 0; i < num_tex; i++)
	{
		fin >> tempname;
		fin >> tempfile;
		mbstowcs(tempwchar, tempfile.c_str(), tempfile.length());
		PrimitiveGenerator::get()->loadTexture(std::wstring(tempwchar), tempname);
	}

	//prefab names, files, count
	fin >> num_prefabs;
	for (int i = 0; i < num_prefabs; i++)
	{
		fin >> tempname;
		fin >> tempfile;
		mbstowcs(tempwchar, tempfile.c_str(), tempfile.length());
		//PrimitiveGenerator::get()->loadTexture(std::wstring(tempwchar), tempname);
		PrefabManager::get()->createPrefab(std::wstring(tempwchar), tempname);
	}
	//

	int numcubes, num_prefab_objects, collider_type;
	Vector3D size, pos, rot, bounding_box, offset;
	std::string diffuse, normal, roughness, prefab_name;
	diffuse.clear();
	normal.clear();
	roughness.clear();

	float x, y, z;
	int shader;

	fin >> numcubes;
	fin >> num_prefab_objects;


	for (int i = 0; i < numcubes; i++)
	{
		fin >> diffuse;
		fin >> normal;
		fin >> roughness;
		fin >> shader;

		fin >> x;
		fin >> y;
		fin >> z;
		size = Vector3D(x, y, z);

		fin >> x;
		fin >> y;
		fin >> z;
		pos = Vector3D(x, y, z);

		fin >> x;
		fin >> y;
		fin >> z;
		rot = Vector3D(x, y, z);

		// fetch material data //
		Material_Obj mat;
		fin >> mat.m_shininess;
		fin >> mat.m_rim_power;
		fin >> mat.m_transparency;

		fin >> mat.m_ambient_color.m_x;
		fin >> mat.m_ambient_color.m_y;
		fin >> mat.m_ambient_color.m_z;
		fin >> mat.m_ambient_color.m_w;

		fin >> mat.m_diffuse_color.m_x;
		fin >> mat.m_diffuse_color.m_y;
		fin >> mat.m_diffuse_color.m_z;
		fin >> mat.m_diffuse_color.m_w;

		fin >> mat.m_specular_color.m_x;
		fin >> mat.m_specular_color.m_y;
		fin >> mat.m_specular_color.m_z;
		fin >> mat.m_specular_color.m_w;

		fin >> mat.m_rim_color.m_x;
		fin >> mat.m_rim_color.m_y;
		fin >> mat.m_rim_color.m_z;
		fin >> mat.m_rim_color.m_w;
		/////////////////////////
		PrimitivePtr p = PrimitiveGenerator::get()->createCube(nullptr, nullptr, nullptr, &mat);
		WorldObjectPtr w = std::shared_ptr<WorldObject>(new WorldObject(p, ColliderTypes::Cube, pos, size, rot));
		m_objects.push_back(w);
		m_objects[i]->getPrimitive()->fetchDiffuseTex(diffuse);
		m_objects[i]->getPrimitive()->fetchNormalTex(normal);
		m_objects[i]->getPrimitive()->fetchRoughnessTex(roughness);
		m_objects[i]->setShader(shader);
	}

	//prefab mesh loading
	for (int i = 0; i < num_prefab_objects; i++)
	{
		////output the prefab name
		//outfile << prefabs[i]->getPrefabName() << " ";
		fin >> prefab_name;

		////output the shader type
		//outfile << prefabs[i]->getShader() << " ";
		fin >> shader;

		fin >> x;
		fin >> y;
		fin >> z;
		size = Vector3D(x, y, z);

		fin >> x;
		fin >> y;
		fin >> z;
		pos = Vector3D(x, y, z);

		fin >> x;
		fin >> y;
		fin >> z;
		rot = Vector3D(x, y, z);

		// fetch material data //
		Material_Obj mat;
		fin >> mat.m_shininess;
		fin >> mat.m_rim_power;
		fin >> mat.m_transparency;

		fin >> mat.m_ambient_color.m_x;
		fin >> mat.m_ambient_color.m_y;
		fin >> mat.m_ambient_color.m_z;
		fin >> mat.m_ambient_color.m_w;

		fin >> mat.m_diffuse_color.m_x;
		fin >> mat.m_diffuse_color.m_y;
		fin >> mat.m_diffuse_color.m_z;
		fin >> mat.m_diffuse_color.m_w;

		fin >> mat.m_specular_color.m_x;
		fin >> mat.m_specular_color.m_y;
		fin >> mat.m_specular_color.m_z;
		fin >> mat.m_specular_color.m_w;

		fin >> mat.m_rim_color.m_x;
		fin >> mat.m_rim_color.m_y;
		fin >> mat.m_rim_color.m_z;
		fin >> mat.m_rim_color.m_w;

		////collider
		std::string is_collider;
		Collider* temp_c = nullptr;
		fin >> is_collider;

		if (is_collider == "TRUE")
		{
			fin >> collider_type;

			fin >> bounding_box.m_x;
			fin >> bounding_box.m_y;
			fin >> bounding_box.m_z;

			fin >> offset.m_x;
			fin >> offset.m_y;
			fin >> offset.m_z;

			switch (collider_type)
			{
			case ColliderTypes::Cube:
				temp_c = new CubeCollider(bounding_box);
				temp_c->setOffset(offset);

				break;
			case ColliderTypes::Sphere:
				break;
			case ColliderTypes::Capsule:
				break;
			}
		}
		else if (is_collider == "FALSE")
		{
			//do nothing
		}

		//make the prefab world object
		PrefabMesh* p_prefab = PrefabManager::get()->findPrefab(prefab_name);

			//m_objects[i]->getPrimitive()->fetchDiffuseTex(diffuse);
		WorldObjectPtr w = std::shared_ptr<WorldObject>(new WorldObject(p_prefab->mesh, temp_c,
			pos, size, rot));
		w->setPrefabName(prefab_name);
		w->setShader(shader);
		w->setMaterial(mat);

		m_objects.push_back(w);

	}


	return true;
}

void WorldObjectManager::clear()
{
	m_objects.clear();
	PrimitiveGenerator::get()->clearTextures();
}

void WorldObjectManager::imGuiRender()
{
	if (m_show_prefab_editor == true)
	{
		if (PrefabManager::get()->ImGuiModifyPrefabs()) m_show_prefab_editor = false;
		return;
	}

	VectorToArray v(&Vector3D());
	ImVec2 button_size = ImVec2(130, 20);

	ImGui::SetNextWindowSize(ImVec2(600, 250));
	ImGui::SetNextWindowBgAlpha(0.4f);
	//ImGui::SetNextWindowPos(ImVec2(0, 515));
	ImGui::Begin("Scene Options");
	//if (ImGui::IsWindowCollapsed()) ImGui::SetWindowPos(ImVec2(0, 765));
	if (ImGui::IsWindowCollapsed()) ImGui::SetWindowPos(ImVec2(0, 765));
	else ImGui::SetWindowPos(ImVec2(0, 515));
	
	ImGui::InputText("Save", m_scenename, ARRAYSIZE(m_scenename));
	ImGui::SameLine();
	if (ImGui::Button("Save Scene", button_size) && m_scenename[0] != 0) outputSceneData(m_scenename);

	ImGui::InputText("Load", m_loadscene, ARRAYSIZE(m_loadscene));
	ImGui::SameLine();
	if (ImGui::Button("Load Scene", button_size) && m_loadscene[0] != 0) loadSceneData(m_loadscene);

	if (ImGui::Button("Clear Scene", button_size)) clear();

	ImGui::NewLine();
	if (ImGui::Button("Edit Prefabs", button_size))
	{
		m_show_prefab_editor = true;
		CameraManager::get()->setCamPos(Vector3D(0, 0, -15.0f));
		CameraManager::get()->setCamRot(Vector2D(0, 0));
	}


	ImGui::End();

	// RESOURCES WINDOW //

	ImGui::SetNextWindowSize(ImVec2(405, 250));
	ImGui::SetNextWindowBgAlpha(0.4f);
	//ImGui::SetNextWindowPos(ImVec2(600, 515));
	ImGui::Begin("Resources");
	if (ImGui::IsWindowCollapsed()) ImGui::SetWindowPos(ImVec2(600, 765));
	else ImGui::SetWindowPos(ImVec2(600, 515));

	if (ImGui::CollapsingHeader("Textures"))
	{
		ImGui::InputText("Load Texture", m_texname, ARRAYSIZE(m_texname));
		if (ImGui::Button("Load") && m_texname[0] != 0)
		{
			int num = PrimitiveGenerator::get()->getTextures().size();

			wchar_t tempwchar[128];
			mbstowcs(tempwchar, m_texname, 128);
			PrimitiveGenerator::get()->loadTexture(std::wstring(tempwchar), "NewTexture(" + std::to_string(num) + ")");
			memset(m_texname, 0, sizeof(m_texname));
		}

		std::vector<TexturePtr> temp_tex = PrimitiveGenerator::get()->getTextures();
		std::vector<std::string> temp_names = PrimitiveGenerator::get()->getTextureNames();

		for (int i = 0; i < temp_tex.size(); i++)
		{
			ImTextureID t = temp_tex[i]->getSRV();
			ImGui::Image(t, ImVec2(40, 40));
			ImGui::SameLine();
			ImGui::Text(temp_names[i].c_str());
		}

	}
	if (ImGui::CollapsingHeader("Meshes"))
	{
		ImGui::InputText("Load Mesh", m_meshname, ARRAYSIZE(m_meshname));
		if (ImGui::Button("Load") && m_meshname[0] != 0)
		{
			int num = PrefabManager::get()->getPrefabs().size();

			wchar_t tempwchar[128];
			mbstowcs(tempwchar, m_meshname, 128);
			PrefabManager::get()->createPrefab(std::wstring(tempwchar), "NewMesh(" + std::to_string(num) + ")");
			memset(m_meshname, 0, sizeof(m_meshname));
		}

		std::vector<std::string> temp_names = PrefabManager::get()->getPrefabNames();

		for (int i = 0; i < temp_names.size(); i++)
		{
			ImGui::Text(temp_names[i].c_str());
		}
	}

	ImGui::End();

	// OBJECT INTERFACE WINDOWS //

	if (m_show_texture_window)
	{
		// TEXTURE SETTINGS //
		ImGui::SetNextWindowSize(ImVec2(220, 300));
		ImGui::SetNextWindowPos(ImVec2(785, 20));
		ImGui::SetNextWindowBgAlpha(0.4f);
		ImGui::Begin("Textures");

		std::vector<TexturePtr> temp_tex = PrimitiveGenerator::get()->getTextures();
		std::vector<std::string> temp_names = PrimitiveGenerator::get()->getTextureNames();

		for (int i = 0; i < temp_tex.size(); i++)
		{
			ImTextureID t = temp_tex[i]->getSRV();
			ImGui::Image(t, ImVec2(40, 40));
			ImGui::SameLine();

			std::string button_name = temp_names[i] + " (" + std::to_string(i) + ")";
			if (ImGui::Button(button_name.c_str()))
			{
				m_objects[m_object_id]->getPrimitive()->loadDiffuseTex(temp_tex[i], temp_names[i]);
			}
		}

		if (ImGui::Button("Accept")) m_show_texture_window = false;

		ImGui::End();
	}
	else if (m_show_shader_window)
	{
		// SHADER SETTINGS //
		ImGui::SetNextWindowSize(ImVec2(220, 300));
		ImGui::SetNextWindowPos(ImVec2(785, 20));
		ImGui::SetNextWindowBgAlpha(0.4f);
		ImGui::Begin("Shaders");

		if (ImGui::Button("Flat", button_size))
		{
			m_objects[m_object_id]->setShader(Shaders::FLAT);
			m_show_shader_window = false;
		}

		if (ImGui::Button("Flat Texture", button_size))
		{
			m_objects[m_object_id]->setShader(Shaders::FLAT_TEX);
			m_show_shader_window = false;
		}

		if (ImGui::Button("Lambert", button_size))
		{
			m_objects[m_object_id]->setShader(Shaders::LAMBERT);
			m_show_shader_window = false;
		}

		if (ImGui::Button("Lambert Rimlight", button_size))
		{
			m_objects[m_object_id]->setShader(Shaders::LAMBERT_RIMLIGHT);
			m_show_shader_window = false;
		}

		if (ImGui::Button("Textured Lambert", button_size))
		{
			m_objects[m_object_id]->setShader(Shaders::TEXTURE);
			m_show_shader_window = false;
		}

		if (ImGui::Button("Triplanar Texturing", button_size))
		{
			m_objects[m_object_id]->setShader(Shaders::TRIPLANAR_TEXTURE);
			m_show_shader_window = false;
		}

		if (ImGui::Button("Accept")) m_show_shader_window = false;

		ImGui::End();
	}
	else if (m_show_mat_window)
	{
		// MATERIAL SETTINGS //
		ImGui::SetNextWindowSize(ImVec2(220, 300));
		ImGui::SetNextWindowPos(ImVec2(785, 20));
		ImGui::SetNextWindowBgAlpha(0.4f);
		ImGui::Begin("Material");

		//ImGui::DragFloat("Transparency", &m_spawn_mat.m_transparency, 0.01f, 0, 1);

		//v = VectorToArray(&m_spawn_mat.m_ambient_color);
		//ImGui::DragFloat4("Ambient Color", v.setArray(), 0.01f, 0.0f, 1.0f);

		v = VectorToArray(&m_focused_material.m_diffuse_color);
		ImGui::DragFloat4("Diffuse Color", v.setArray(), 0.01f, 0.0f, 1.0f);

		v = VectorToArray(&m_focused_material.m_specular_color);
		ImGui::DragFloat4("Specular Color", v.setArray(), 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Shininess", &m_focused_material.m_shininess, 0.05f, 0, 40);

		v = VectorToArray(&m_focused_material.m_rim_color);
		ImGui::DragFloat4("RimLight Color", v.setArray(), 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("RimLight Amount", &m_focused_material.m_rim_power, 0.05f, 0, 40);

		m_objects[m_object_id]->getPrimitive()->setMaterial(m_focused_material);

		if (ImGui::Button("Accept")) m_show_mat_window = false;

		ImGui::End();
	}
	else if (m_show_obj_window)
	{
		// GENERAL // 
		ImGui::SetNextWindowSize(ImVec2(220, 300));
		ImGui::SetNextWindowPos(ImVec2(785, 20));
		ImGui::SetNextWindowBgAlpha(0.4f);

		ImGui::Begin("Object Settings");

		v = VectorToArray(&m_focused_pos);
		ImGui::DragFloat3("Position", v.setArray(), 0.05f, -100.0f, 100.0f);
		v = VectorToArray(&m_focused_scale);
		ImGui::DragFloat3("Scale", v.setArray(), 0.05f, -100.0f, 100.0f);
		v = VectorToArray(&m_focused_rot);
		ImGui::DragFloat3("Rotation", v.setArray(), 0.01f, -100.0f, 100.0f);



		if (m_objects.size())
		{
			m_objects[m_object_id]->setPosition(m_focused_pos);
			m_objects[m_object_id]->setScale(m_focused_scale);
			m_objects[m_object_id]->setRotation(m_focused_rot);
		}

		ImVec2 button_size = ImVec2(200, 20);
		if (ImGui::Button("Shader", button_size)) m_show_shader_window = true;
		if (ImGui::Button("Texture", button_size)) m_show_texture_window = true;
		

		if (ImGui::Button("Material Properties", button_size))
		{
			m_show_mat_window = true;
			m_focused_material = m_objects[m_object_id]->getMaterial();
		}

		Collider* c = m_objects[m_object_id]->getCollider();
		if (c == nullptr)
		{
			ImGui::NewLine();
			if (ImGui::Button("Add Collider", button_size))
			{
				Collider* temp = new CubeCollider(Vector3D(1, 1, 1));
				m_objects[m_object_id]->setCollider(temp);
				m_objects[m_object_id]->toggleBoundingBoxRender();
			}
		}
		else
		{
			bool temp_bool = m_objects[m_object_id]->getBoundingBoxRenderState();
			if (ImGui::Checkbox("Show Collider", &temp_bool)) m_objects[m_object_id]->toggleBoundingBoxRender();
			if (temp_bool)
			{
				ImGui::PushID("Collider");
				Vector3D bounding_box = m_objects[m_object_id]->getCollider()->getBoundingBox();
				v = VectorToArray(&bounding_box);
				ImGui::DragFloat3("Size", v.setArray(), 0.025f, 0.01);
				m_objects[m_object_id]->getCollider()->setBoundingBox(bounding_box);

				Vector3D offset = m_objects[m_object_id]->getCollider()->getOffset();
				v = VectorToArray(&offset);
				ImGui::DragFloat3("Offset", v.setArray(), 0.025f, 0.01);
				m_objects[m_object_id]->getCollider()->setOffset(offset);
				ImGui::PopID();
			}
		}

		ImGui::NewLine();
		ImGui::NewLine();
		if (ImGui::Button("Accept")) m_show_obj_window = false;

		ImGui::End();
	}


	// OBJECT LIST //
	ImGui::SetNextWindowSize(ImVec2(150, 250));
	ImGui::SetNextWindowPos(ImVec2(0, 65));
	ImGui::SetNextWindowBgAlpha(0.4f);
	ImGui::Begin("Scene Objects");

	for (int i = 0; i < m_objects.size(); i++)
	{
		std::string name = "Object " + std::to_string(i);
		if (ImGui::Button(name.c_str(), button_size))
		{
			m_show_obj_window = true;
			m_focused_pos = m_objects[i]->getPosition();
			m_focused_scale = m_objects[i]->getScale();
			m_focused_rot = m_objects[i]->getRotation();
			m_object_id = i;
		}
	}

	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(150, 200));
	ImGui::SetNextWindowPos(ImVec2(0, 315));
	ImGui::SetNextWindowBgAlpha(0.4f);
	ImGui::Begin("Create New");

	if (ImGui::Button("Spawn Cube"))
	{
		m_show_obj_window = true;
		m_focused_pos = Vector3D(0, 0, 0);
		m_focused_scale = Vector3D(1, 1, 1);
		m_focused_rot = Vector3D(0, 0, 0);

		PrimitivePtr p = PrimitiveGenerator::get()->createCube(nullptr, nullptr, nullptr, nullptr);
		WorldObjectPtr w = std::shared_ptr<WorldObject>(new WorldObject(p, ColliderTypes::Cube, m_focused_scale, m_focused_pos, m_focused_rot));
		m_objects.push_back(w);

		m_object_id = m_objects.size() - 1;
	}

	std::vector<std::string> temp_names = PrefabManager::get()->getPrefabNames();

	for (int i = 0; i < temp_names.size(); i++)
	{
		if (ImGui::Button(("Spawn " + temp_names[i]).c_str()))
		{

			std::vector<PrefabMesh> temp = PrefabManager::get()->getPrefabs();

			m_show_obj_window = true;
			m_focused_pos = Vector3D(0, 0, 0);
			m_focused_scale = temp[i].default_scale;
			m_focused_rot = temp[i].default_rot;

			WorldObjectPtr w = std::shared_ptr<WorldObject>(new WorldObject(temp[i].mesh, nullptr, 
				m_focused_pos, m_focused_scale, m_focused_rot));
			w->setPrefabName(temp_names[i]);
			w->setShader(temp[i].default_shader);
			w->setMaterial(temp[i].mesh->getMaterial());
			w->setCollider(temp[i].collider);

			m_objects.push_back(w);

			m_object_id = m_objects.size() - 1;
		}
	}


	ImGui::End();




}

void WorldObjectManager::render(float elapsed_time, bool skip_highlight)
{
	for (int i = 0; i < m_objects.size(); i++)
	{
		//for the highlight renderer, we will skip any models that need highlighting here
		if (skip_highlight && m_show_obj_window == true && i == m_object_id) continue;

		m_objects[i]->render(elapsed_time, -1, true);
	}
}

void WorldObjectManager::renderSelectedHighlight()
{
	if (m_objects.size() < 1 || m_show_obj_window != true) return;

	//set the blend to screen
	//BlendMode::get()->SetBlend(SCREEN);
	
	Vector3D s = m_objects[m_object_id]->getScale();
	Vector3D p = m_objects[m_object_id]->getPosition();
	Vector3D r = m_objects[m_object_id]->getRotation();

	//get the material of the object
	Material_Obj m = m_objects[m_object_id]->getMaterial();
	//save the original transparency of the object
	Vector4D temp = m.m_diffuse_color;
	//reduce the transparency before rendering
	m.m_diffuse_color = temp * 1.1f + 0.2f;
	m_objects[m_object_id]->setMaterial(m);

	//render
	m_objects[m_object_id]->render(0, s, p, r, LAMBERT, false);

	//return settings to how they were
	//BlendMode::get()->SetBlend(ALPHA);
	m.m_diffuse_color = temp;
	m_objects[m_object_id]->setMaterial(m);
}

void WorldObjectManager::renderBoundingBoxes()
{
	//temporary code//
	/* this function is only being used short term for testing and will be reworked. */

	//create a cube to render bounding boxes.  temporary
	PrimitivePtr cube = PrimitiveGenerator::get()->createCube(nullptr, nullptr, nullptr, nullptr);

	//GraphicsEngine::get()->getShaderManager()->setPipeline(Shaders::FLAT);

	Material_Obj m = cube->getMaterial();
	//save the original transparency of the object
	Vector4D temp = m.m_diffuse_color;
	//reduce the transparency before rendering
	m.m_diffuse_color = Vector4D(0.4f, 0.2f, 0.2f, 1.0f);
	cube->setMaterial(m);

	Collider* c = nullptr;
	Vector3D s;
	Vector3D p;
	Vector3D r = {};

	//set the blend to screen
	BlendMode::get()->SetBlend(SCREEN);

	for (int i = 0; i < m_objects.size(); i++)
	{
		if (!m_objects[i]->getBoundingBoxRenderState()) continue;
		c = m_objects[i]->getCollider();
		if (c == nullptr) continue;

		s = c->getBoundingBox() + 0.02f;
		p = m_objects[i]->getPosition() + c->getOffset();

		cube->render(s, p, r, Shaders::FLAT);
	}

	//return settings to how they were
	BlendMode::get()->SetBlend(ALPHA);
}

Vector3D WorldObjectManager::BoundingBoxCollision(Vector3D old_pos, Vector3D new_pos, Vector3D size)
{
	Vector3D move = new_pos - old_pos;

	Vector3D halfsize = size / 2.0f;
	Vector3D pos2;
	Vector3D halfsize2;
	Collider* col = nullptr;

	for (int i = 0; i < m_objects.size(); i++)
	{
		//temp
		//if (m_objects[i]->getObjectType() == ObjectType::Mesh) continue;
		col = m_objects[i]->getCollider();
		if (col == nullptr) continue;
		//pos2 = m_cubes[i]->getPosition();
		//halfsize2 = m_cubes[i]->getScale() / 2.0f;

		////立方体同士の当たり判定
		//Vector3D dif = pos2 - new_pos;
		//if (abs(dif.m_x) > (halfsize.m_x + halfsize2.m_x)) continue;
		//if (abs(dif.m_y) > (halfsize.m_y + halfsize2.m_y)) continue;
		//if (abs(dif.m_z) > (halfsize.m_z + halfsize2.m_z)) continue;

		pos2 = m_objects[i]->getPosition() + col->getOffset();
		halfsize2 = col->getBoundingBox() / 2.0f;

		//立方体同士の当たり判定
		Vector3D dif = new_pos - pos2;
		//新しい位置が当たらない場合はコンティニュー
		if (abs(dif.m_x) > (halfsize.m_x + halfsize2.m_x)) continue;
		if (abs(dif.m_y) > (halfsize.m_y + halfsize2.m_y)) continue;
		if (abs(dif.m_z) > (halfsize.m_z + halfsize2.m_z)) continue;


		//当たった場合、解決する
		Vector3D result = old_pos + Vector3D(move.m_x, 0, 0);
		dif = result - pos2;
		bool collide = true;

		//if (abs(dif.m_x) > (halfsize.m_x + halfsize2.m_x)) collide = false;
		if ((result.m_x - halfsize.m_x >= pos2.m_x - halfsize2.m_x || move.m_x <= 0) &&
			(result.m_x + halfsize.m_x <= pos2.m_x + halfsize2.m_x || move.m_x >= 0)) collide = false;
		//if () collide = false;

		//if (abs(dif.m_x) < (halfsize.m_x + halfsize2.m_x) &&
		//	abs(dif.m_y) < (halfsize.m_y + halfsize2.m_y) &&
		//	abs(dif.m_z) < (halfsize.m_z + halfsize2.m_z)) collide = true;

		//X movement collision
		if (collide)
		{
			if (dif.m_x < 0) result.m_x = pos2.m_x - (halfsize.m_x + halfsize2.m_x + 0.01f);
			else result.m_x = pos2.m_x + (halfsize.m_x + halfsize2.m_x + 0.01f);
		}

		collide = true;
		result += Vector3D(0, move.m_y, 0);
		dif = result - pos2;

		//if (abs(dif.m_y) > (halfsize.m_y + halfsize2.m_y)) collide = false;
		if ((result.m_y - halfsize.m_y >= pos2.m_y - halfsize2.m_y || move.m_y <= 0) &&
			(result.m_y + halfsize.m_y <= pos2.m_y + halfsize2.m_y || move.m_y >= 0)) collide = false;

		//Y movement collision
		if (collide)
		{
			if (dif.m_y < 0) result.m_y = pos2.m_y - (halfsize.m_y + halfsize2.m_y + 0.01f);
			else result.m_y = pos2.m_y + (halfsize.m_y + halfsize2.m_y + 0.01f);
		}

		collide = true;
		result += Vector3D(0, 0, move.m_z);
		dif = result - pos2;

		//if (abs(dif.m_z) > (halfsize.m_z + halfsize2.m_z)) collide = false;
		if ((result.m_z - halfsize.m_z >= pos2.m_z - halfsize2.m_z || move.m_z <= 0) &&
			(result.m_z + halfsize.m_z <= pos2.m_z + halfsize2.m_z || move.m_z >= 0)) collide = false;

		//Z movement collision
		if (collide)
		{
			if (dif.m_z < 0) result.m_z = pos2.m_z - (halfsize.m_z + halfsize2.m_z + 0.01f);
			else result.m_z = pos2.m_z + (halfsize.m_z + halfsize2.m_z + 0.01f);
		}

		return result;
	}

	return new_pos;
}
