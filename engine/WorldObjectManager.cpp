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
#include "Collision.h"

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

	if (current_file == filename) return;
	else current_file = filename;

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

		memset(&tempchar, 0, sizeof(char) * 1024);
		texfile.clear();
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

		memset(&tempchar, 0, sizeof(char) * 1024);
		prefabfile.clear();
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
		tempfile.clear();
		tempname.clear();
		memset(&tempwchar, 0, sizeof(wchar_t) * 128);
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
		tempfile.clear();
		tempname.clear();
		memset(&tempwchar, 0, sizeof(wchar_t) * 128);
	}
	//

	int numcubes, num_prefab_objects, collider_type;
	Vec3 size, pos, rot, bounding_box, offset;
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
		size = Vec3(x, y, z);

		fin >> x;
		fin >> y;
		fin >> z;
		pos = Vec3(x, y, z);

		fin >> x;
		fin >> y;
		fin >> z;
		rot = Vec3(x, y, z);

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
		PrimitivePtr p = PrimitiveGenerator::get()->createUnitCube(nullptr, nullptr, nullptr, &mat);
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
		size = Vec3(x, y, z);

		fin >> x;
		fin >> y;
		fin >> z;
		pos = Vec3(x, y, z);

		fin >> x;
		fin >> y;
		fin >> z;
		rot = Vec3(x, y, z);

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

			fin >> bounding_box.x;
			fin >> bounding_box.y;
			fin >> bounding_box.z;

			fin >> offset.x;
			fin >> offset.y;
			fin >> offset.z;

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
	m_show_obj_window = false;
}

void WorldObjectManager::imGuiRender()
{
	if (m_show_prefab_editor == true)
	{
		if (PrefabManager::get()->ImGuiModifyPrefabs()) m_show_prefab_editor = false;
		return;
	}

	VectorToArray v(&Vec3());
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
		CameraManager::get()->setCamPos(Vec3(0, 0, -15.0f));
		CameraManager::get()->setCamRot(Vec2(0, 0));
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
		ImGui::DragFloat3("Diffuse Color", v.setArray(), 0.01f, 0.0f, 1.0f);

		v = VectorToArray(&m_focused_material.m_specular_color);
		ImGui::DragFloat3("Specular Color", v.setArray(), 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Shininess", &m_focused_material.m_shininess, 0.05f, 0, 40);

		v = VectorToArray(&m_focused_material.m_rim_color);
		ImGui::DragFloat3("RimLight Color", v.setArray(), 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("RimLight Amount", &m_focused_material.m_rim_power, 0.05f, 0, 40);

		m_objects[m_object_id]->setMaterial(m_focused_material);

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
				Collider* temp = new CubeCollider(Vec3(1, 1, 1));
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
				Vec3 bounding_box = m_objects[m_object_id]->getCollider()->getBoundingBox();
				v = VectorToArray(&bounding_box);
				ImGui::DragFloat3("Size", v.setArray(), 0.025f, 0.01);
				m_objects[m_object_id]->getCollider()->setBoundingBox(bounding_box);

				Vec3 offset = m_objects[m_object_id]->getCollider()->getOffset();
				v = VectorToArray(&offset);
				ImGui::DragFloat3("Offset", v.setArray(), 0.025f, 0.01);
				m_objects[m_object_id]->getCollider()->setOffset(offset);
				ImGui::PopID();
			}
		}

		ImGui::NewLine();
		ImGui::NewLine();
		if (ImGui::Button("Accept")) m_show_obj_window = false;
		ImGui::SameLine(0, 75.0f);
		if (ImGui::Button("Delete"))
		{
			m_objects.erase(m_objects.begin() + m_object_id);
			m_show_obj_window = false;
		}

		ImGui::End();
	}


	// OBJECT LIST //
	ImGui::SetNextWindowSize(ImVec2(150, 210));
	ImGui::SetNextWindowPos(ImVec2(0, 102));
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
		m_focused_pos = Vec3(0, 0, 0);
		m_focused_scale = Vec3(1, 1, 1);
		m_focused_rot = Vec3(0, 0, 0);

		PrimitivePtr p = PrimitiveGenerator::get()->createUnitCube(nullptr, nullptr, nullptr, nullptr);
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
			m_focused_pos = Vec3(0, 0, 0);
			m_focused_scale = temp[i].default_scale;
			m_focused_rot = temp[i].default_rot;

			WorldObjectPtr w = std::shared_ptr<WorldObject>(new WorldObject(temp[i].mesh, nullptr, 
				m_focused_pos, m_focused_scale, m_focused_rot));
			w->setPrefabName(temp_names[i]);
			w->setShader(temp[i].default_shader);
			w->setMaterial(temp[i].mesh->getMaterial());

			Collider* collider = nullptr;

			if (temp[i].collider != nullptr)
			{
				switch (temp[i].collider->getType())
				{
				case ColliderTypes::Cube:
					collider = new CubeCollider(*temp[i].collider);
					break;
				}
			}
			w->setCollider(collider);

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
	
	Vec3 s = m_objects[m_object_id]->getScale();
	Vec3 p = m_objects[m_object_id]->getPosition();
	Vec3 r = m_objects[m_object_id]->getRotation();

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

void WorldObjectManager::renderBoundingBoxes(bool force_show)
{
	//temporary code//
	/* this function is only being used short term for testing and will be reworked. */

	//create a cube to render bounding boxes.  temporary
	PrimitivePtr cube = PrimitiveGenerator::get()->createUnitCube(nullptr, nullptr, nullptr, nullptr);

	//GraphicsEngine::get()->getShaderManager()->setPipeline(Shaders::FLAT);

	Material_Obj m = cube->getMaterial();
	//save the original transparency of the object
	Vector4D temp = m.m_diffuse_color;
	//reduce the transparency before rendering
	m.m_diffuse_color = Vector4D(0.4f, 0.2f, 0.2f, 1.0f);
	cube->setMaterial(m);

	Collider* c = nullptr;
	Vec3 s;
	Vec3 p;
	Vec3 r = {};

	//set the blend to screen
	BlendMode::get()->SetBlend(SCREEN);

	for (int i = 0; i < m_objects.size(); i++)
	{
		if (!force_show)
		{
			if (!m_objects[i]->getBoundingBoxRenderState()) continue;
		}

		c = m_objects[i]->getCollider();
		if (c == nullptr) continue;

		s = c->getBoundingBox() + 0.02f;
		p = m_objects[i]->getPosition() + c->getOffset();

		cube->render(s, p, r, Shaders::FLAT);
	}

	//return settings to how they were
	BlendMode::get()->SetBlend(ALPHA);
}

Vec3 WorldObjectManager::BBoxCollisionResolveDiscrete(Vec3 old_pos, Vec3 new_pos, Vec3 size)
{
	Vec3 move = new_pos - old_pos;

	Vec3 halfsize = size / 2.0f;
	Vec3 pos2;
	Vec3 halfsize2;
	Collider* col = nullptr;

	for (int i = 0; i < m_objects.size(); i++)
	{
		//temp
		//if (m_objects[i]->getObjectType() == ObjectType::Mesh) continue;
		col = m_objects[i]->getCollider();
		if (col == nullptr) continue;

		pos2 = m_objects[i]->getPosition() + col->getOffset();
		halfsize2 = col->getBoundingBox() / 2.0f;

		//立方体同士の当たり判定
		Vec3 dif = new_pos - pos2;
		//新しい位置が当たらない場合はコンティニュー
		if (abs(dif.x) > (halfsize.x + halfsize2.x)) continue;
		if (abs(dif.y) > (halfsize.y + halfsize2.y)) continue;
		if (abs(dif.z) > (halfsize.z + halfsize2.z)) continue;


		//当たった場合、解決する
		Vec3 result = old_pos + Vec3(move.x, 0, 0);
		dif = result - pos2;
		bool collide = true;

		//if (abs(dif.m_x) > (halfsize.m_x + halfsize2.m_x)) collide = false;
		if ((result.x - halfsize.x >= pos2.x - halfsize2.x || move.x <= 0) &&
			(result.x + halfsize.x <= pos2.x + halfsize2.x || move.x >= 0)) collide = false;

		//X movement collision
		if (collide)
		{
			if (dif.x < 0) result.x = pos2.x - (halfsize.x + halfsize2.x + 0.01f);
			else result.x = pos2.x + (halfsize.x + halfsize2.x + 0.01f);
		}

		collide = true;
		result += Vec3(0, move.y, 0);
		dif = result - pos2;

		//if (abs(dif.m_y) > (halfsize.m_y + halfsize2.m_y)) collide = false;
		if ((result.y - halfsize.y >= pos2.y - halfsize2.y || move.y <= 0) &&
			(result.y + halfsize.y <= pos2.y + halfsize2.y || move.y >= 0)) collide = false;

		//Y movement collision
		if (collide)
		{
			if (dif.y < 0) result.y = pos2.y - (halfsize.y + halfsize2.y + 0.01f);
			else result.y = pos2.y + (halfsize.y + halfsize2.y + 0.01f);
		}

		collide = true;
		result += Vec3(0, 0, move.z);
		dif = result - pos2;

		//if (abs(dif.m_z) > (halfsize.m_z + halfsize2.m_z)) collide = false;
		if ((result.z - halfsize.z >= pos2.z - halfsize2.z || move.z <= 0) &&
			(result.z + halfsize.z <= pos2.z + halfsize2.z || move.z >= 0)) collide = false;

		//Z movement collision
		if (collide)
		{
			if (dif.z < 0) result.z = pos2.z - (halfsize.z + halfsize2.z + 0.01f);
			else result.z = pos2.z + (halfsize.z + halfsize2.z + 0.01f);
		}

		return result;
	}

	return new_pos;
}

Vec3 WorldObjectManager::BBoxCollisionResolveCont(Vec3 old_pos, Vec3 new_pos, Vec3 size)
{
	Vec3 move = new_pos - old_pos;
	float speed = move.length();

	Vec3 halfsize = size / 2.0f;
	Vec3 pos2;
	Vec3 halfsize2;
	Collider* col = nullptr;
	float time_x = -1;
	float time_y = -1;
	float time_z = -1;
	//float lowest_time = 1000;
	bool hit = false;

	float pos1_val, pos2_val;
	Vec3 adjusted_pos;

	for (int i = 0; i < m_objects.size(); i++)
	{
		adjusted_pos = new_pos;
		//temp
		//if (m_objects[i]->getObjectType() == ObjectType::Mesh) continue;
		col = m_objects[i]->getCollider();
		if (col == nullptr) continue;

		pos2 = m_objects[i]->getPosition() + col->getOffset();
		halfsize2 = col->getBoundingBox() / 2.0f;

		//立方体同士の当たり判定

		//X軸の時間差
		if (move.x > 0)
		{
			pos1_val = old_pos.x + halfsize.x;
			pos2_val = pos2.x - halfsize2.x;
		}
		else
		{
			pos1_val = old_pos.x - halfsize.x;
			pos2_val = pos2.x + halfsize2.x;
		}
		time_x = (pos2_val - pos1_val) / move.x;

		//Y軸の時間差
		if (move.y > 0)
		{
			pos1_val = old_pos.y + halfsize.y;
			pos2_val = pos2.y - halfsize2.y;
		}
		else
		{
			pos1_val = old_pos.y - halfsize.y;
			pos2_val = pos2.y + halfsize2.y;
		}
		time_y = (pos2_val - pos1_val) / move.y;

		//Z軸の時間差
		if (move.z > 0)
		{
			pos1_val = old_pos.z + halfsize.z;
			pos2_val = pos2.z - halfsize2.z;
		}
		else
		{
			pos1_val = old_pos.z - halfsize.z;
			pos2_val = pos2.z + halfsize2.z;
		}
		time_z = (pos2_val - pos1_val) / move.z;


		//衝突の判定
		//move.normalize();
		Vec3 min2 = Vec3(pos2 - halfsize2);
		Vec3 max2 = Vec3(pos2 + halfsize2);
		if (time_x < 1.0f && time_x > 0)
		{
			Vec3 min1 = old_pos + move * time_x * 1.01f - halfsize;
			Vec3 max1 = old_pos + move * time_x * 1.01f + halfsize;
			if (Collision::DetectAABBDiscrete(min1, max1, min2, max2))
			{
				adjusted_pos.x = pos2.x - (halfsize2.x + halfsize.x + 0.01f) * ((move.x > 0) * 2 - 1);
				hit = true;
			}
		}
		//if (time_y < speed && time_y > 0 && time_y < lowest_time) lowest_time = time_y;
		//if (time_z < speed && time_z > 0 && time_z < lowest_time) lowest_time = time_z;
		if (time_y < 1.0f && time_y > 0)
		{
			Vec3 min1 = old_pos + move * time_y * 1.01f - halfsize;
			Vec3 max1 = old_pos + move * time_y * 1.01f + halfsize;
			if (Collision::DetectAABBDiscrete(min1, max1, min2, max2))
			{
				adjusted_pos.y = pos2.y - (halfsize2.y + halfsize.y + 0.01f) * ((move.y > 0) * 2 - 1);
				hit = true;
			}
		}

		if (time_z < 1.0f && time_z > 0)
		{
			Vec3 min1 = old_pos + move * time_z * 1.01f - halfsize;
			Vec3 max1 = old_pos + move * time_z * 1.01f + halfsize;
			if (Collision::DetectAABBDiscrete(min1, max1, min2, max2))
			{
				adjusted_pos.z = pos2.z - (halfsize2.z + halfsize.z + 0.01f) * ((move.z > 0) * 2 - 1);
				hit = true;
			}
		}

		if (hit)
		{
 			return adjusted_pos;
		}
	}

	return new_pos;
}

bool WorldObjectManager::BBoxCollisionCheckCont(Vec3 old_pos, Vec3 new_pos, Vec3 size)
{
	Vec3 move = new_pos - old_pos;
	float speed = move.length();

	Vec3 halfsize = size / 2.0f;
	Vec3 pos2;
	Vec3 halfsize2;
	Collider* col = nullptr;
	float time_x = -1;
	float time_y = -1;
	float time_z = -1;
	//float lowest_time = 1000;
	bool hit = false;

	float pos1_val, pos2_val;
	Vec3 adjusted_pos;

	for (int i = 0; i < m_objects.size(); i++)
	{
		adjusted_pos = new_pos;
		//temp
		//if (m_objects[i]->getObjectType() == ObjectType::Mesh) continue;
		col = m_objects[i]->getCollider();
		if (col == nullptr) continue;

		pos2 = m_objects[i]->getPosition() + col->getOffset();
		halfsize2 = col->getBoundingBox() / 2.0f;

		//立方体同士の当たり判定

		//X軸の時間差
		if (move.x > 0)
		{
			pos1_val = old_pos.x + halfsize.x;
			pos2_val = pos2.x - halfsize2.x;
		}
		else
		{
			pos1_val = old_pos.x - halfsize.x;
			pos2_val = pos2.x + halfsize2.x;
		}
		time_x = (pos2_val - pos1_val) / move.x;

		//Y軸の時間差
		if (move.y > 0)
		{
			pos1_val = old_pos.y + halfsize.y;
			pos2_val = pos2.y - halfsize2.y;
		}
		else
		{
			pos1_val = old_pos.y - halfsize.y;
			pos2_val = pos2.y + halfsize2.y;
		}
		time_y = (pos2_val - pos1_val) / move.y;

		//Z軸の時間差
		if (move.z > 0)
		{
			pos1_val = old_pos.z + halfsize.z;
			pos2_val = pos2.z - halfsize2.z;
		}
		else
		{
			pos1_val = old_pos.z - halfsize.z;
			pos2_val = pos2.z + halfsize2.z;
		}
		time_z = (pos2_val - pos1_val) / move.z;

		//if (time_x < 1.0f || time_x > 0 &&
		//	time_y < 1.0f || time_y > 0 &&
		//	time_z < 1.0f || time_z > 0) return true;
		Vec3 min2 = Vec3(pos2 - halfsize2);
		Vec3 max2 = Vec3(pos2 + halfsize2);
		if (time_x < 1.0f && time_x > 0)
		{
			Vec3 min1 = old_pos + move * time_x * 1.01f - halfsize;
			Vec3 max1 = old_pos + move * time_x * 1.01f + halfsize;
			if (Collision::DetectAABBDiscrete(min1, max1, min2, max2))
			{
				return true;
			}
		}
		//if (time_y < speed && time_y > 0 && time_y < lowest_time) lowest_time = time_y;
		//if (time_z < speed && time_z > 0 && time_z < lowest_time) lowest_time = time_z;
		if (time_y < 1.0f && time_y > 0)
		{
			Vec3 min1 = old_pos + move * time_y * 1.01f - halfsize;
			Vec3 max1 = old_pos + move * time_y * 1.01f + halfsize;
			if (Collision::DetectAABBDiscrete(min1, max1, min2, max2))
			{
				return true;
			}
		}

		if (time_z < 1.0f && time_z > 0)
		{
			Vec3 min1 = old_pos + move * time_z * 1.01f - halfsize;
			Vec3 max1 = old_pos + move * time_z * 1.01f + halfsize;
			if (Collision::DetectAABBDiscrete(min1, max1, min2, max2))
			{
				return true;
			}
		}
	
	}

	return false;
}
