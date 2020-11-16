#include "WorldObjectManager.h"
#include "Texture.h"
#include "Cube.h"
#include "VectorToArray.h"

#include <iostream>
#include <fstream>  

WorldObjectManager* WorldObjectManager::instance = nullptr;

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
	char tempchar[128] = {};
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

	//number of cubes
	outfile << std::to_string(m_cubes.size()) << " ";

	Vector4D temp;

	/* PER CUBE */
	for (int i = 0; i < m_cubes.size(); i++)
	{
		//output the diffuse texname
		outfile << m_cubes[i]->getDiffuseName() << " ";
		//output the normal texname
		outfile << m_cubes[i]->getNormalName() << " ";
		//output the roughness texname
		outfile << m_cubes[i]->getRoughnessName() << " ";
		//output the shader type
		outfile << m_cubes[i]->getShader() << " ";

		//output the scale
		temp = m_cubes[i]->getScale();
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";

		//output the position
		temp = m_cubes[i]->getPosition();
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";

		//output the rotation
		temp = m_cubes[i]->getRotation();
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";

		/* material data */
		outfile << std::to_string(m_cubes[i]->getMaterial().m_shininess) << " ";
		outfile << std::to_string(m_cubes[i]->getMaterial().m_rim_power) << " ";
		outfile << std::to_string(m_cubes[i]->getMaterial().m_transparency) << " ";

		temp = m_cubes[i]->getMaterial().m_ambient_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		temp = m_cubes[i]->getMaterial().m_diffuse_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		temp = m_cubes[i]->getMaterial().m_specular_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";

		temp = m_cubes[i]->getMaterial().m_rim_color;
		outfile << std::to_string(temp.m_x) << " ";
		outfile << std::to_string(temp.m_y) << " ";
		outfile << std::to_string(temp.m_z) << " ";
		outfile << std::to_string(temp.m_w) << " ";
		/* ------------------------------ */

	}

	outfile.close();
}

void WorldObjectManager::loadSceneData(std::string filename)
{
	m_cubes.clear();
	PrimitiveGenerator::get()->clearTextures();

	std::string file = filename;
	std::ifstream fin;

	fin = std::ifstream("..\\Assets\\SceneData\\" + file);

	_ASSERT_EXPR(fin.is_open(), L"Scene Data file not found!");

	std::vector<TexturePtr> temp_tex;
	std::vector<std::string> temp_texname;
	std::vector<std::wstring> temp_texfile;

	int num_tex;
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

	int numcubes;
	Vector3D size, pos, rot;
	std::string diffuse, normal, roughness;
	diffuse.clear();
	normal.clear();
	roughness.clear();

	float x, y, z;
	int shader;

	fin >> numcubes;
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

		m_cubes.push_back(PrimitiveGenerator::get()->createCube(nullptr, nullptr, nullptr, size, pos, rot, &mat));
		m_cubes[i]->fetchDiffuseTex(diffuse);
		m_cubes[i]->fetchNormalTex(normal);
		m_cubes[i]->fetchRoughnessTex(roughness);
		m_cubes[i]->setShader(shader);
	}
}

void WorldObjectManager::clear()
{
	m_cubes.clear();
	PrimitiveGenerator::get()->clearTextures();
}

void WorldObjectManager::imGuiRender()
{
	VectorToArray v(&Vector3D());

	if (ImGui::Button("Spawn Cube"))
	{
		m_show_obj_window = true;
		m_spawn_pos = Vector3D(0, 0, 0);
		m_spawn_scale = Vector3D(1, 1, 1);
		m_spawn_rot = Vector3D(0, 0, 0);

		m_cubes.push_back(PrimitiveGenerator::get()->createCube(nullptr, nullptr, nullptr, m_spawn_scale, m_spawn_pos, m_spawn_rot, nullptr));

		m_cube_ID = m_cubes.size() - 1;
	}
	ImGui::InputText("Save", m_scenename, ARRAYSIZE(m_scenename));
	ImGui::SameLine();
	if (ImGui::Button("Save Scene") && m_scenename[0] != 0) outputSceneData(m_scenename);

	ImGui::InputText("Load", m_loadscene, ARRAYSIZE(m_loadscene));
	ImGui::SameLine();
	if (ImGui::Button("Load Scene") && m_loadscene[0] != 0) loadSceneData(m_loadscene);

	if (ImGui::Button("Clear Scene")) clear();

	if (m_show_obj_window)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 200));
		ImGui::SetNextWindowPos(ImVec2(105, 250));
		ImGui::Begin("Object Settings");

		v = VectorToArray(&m_spawn_pos);
		ImGui::DragFloat3("Position", v.setArray(), 0.05f, -100.0f, 100.0f);
		v = VectorToArray(&m_spawn_scale);
		ImGui::DragFloat3("Scale", v.setArray(), 0.05f, -100.0f, 100.0f);
		v = VectorToArray(&m_spawn_rot);
		ImGui::DragFloat3("Rot", v.setArray(), 0.01f, -100.0f, 100.0f);

		if (m_cubes.size())
		{
			m_cubes[m_cube_ID]->setPosition(m_spawn_pos);
			m_cubes[m_cube_ID]->setScale(m_spawn_scale);
			m_cubes[m_cube_ID]->setRotation(m_spawn_rot);
		}

		if (ImGui::Button("Shader")) m_show_shader_window = true;

		if (m_show_shader_window)
		{
			ImGui::SetNextWindowSize(ImVec2(200, 200));
			ImGui::SetNextWindowPos(ImVec2(105, 250));
			ImGui::Begin("Shaders");

			if (ImGui::Button("Flat"))
			{
				m_cubes[m_cube_ID]->setShader(Shaders::FLAT);
				m_show_shader_window = false;
			}

			if (ImGui::Button("Flat Texture"))
			{
				m_cubes[m_cube_ID]->setShader(Shaders::FLAT_TEX);
				m_show_shader_window = false;
			}

			if (ImGui::Button("Lambert"))
			{
				m_cubes[m_cube_ID]->setShader(Shaders::LAMBERT);
				m_show_shader_window = false;
			}

			if (ImGui::Button("Lambert Rimlight"))
			{
				m_cubes[m_cube_ID]->setShader(Shaders::LAMBERT_RIMLIGHT);
				m_show_shader_window = false;
			}

			if (ImGui::Button("Textured Lambert"))
			{
				m_cubes[m_cube_ID]->setShader(Shaders::TEXTURE);
				m_show_shader_window = false;
			}

			if (ImGui::Button("Triplanar Texturing"))
			{
				m_cubes[m_cube_ID]->setShader(Shaders::TRIPLANAR_TEXTURE);
				m_show_shader_window = false;
			}

			if (ImGui::Button("Accept")) m_show_shader_window = false;

			ImGui::End();
		}

		if (ImGui::Button("Texture")) m_show_texture_window = true;
		if (m_show_texture_window)
		{
			ImGui::SetNextWindowSize(ImVec2(200, 200));
			ImGui::SetNextWindowPos(ImVec2(105, 250));
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
					m_cubes[m_cube_ID]->loadDiffuseTex(temp_tex[i], temp_names[i]);
				}
			}

			if (ImGui::Button("Accept")) m_show_texture_window = false;

			ImGui::End();
		}

		if (ImGui::Button("Material Properties"))
		{
			m_show_mat_window = true;
			m_spawn_mat = m_cubes[m_cube_ID]->getMaterial();

		}
		if (m_show_mat_window)
		{
			ImGui::SetNextWindowSize(ImVec2(200, 200));
			ImGui::SetNextWindowPos(ImVec2(105, 250));
			ImGui::Begin("Material");

			ImGui::DragFloat("Transparency", &m_spawn_mat.m_transparency, 0.01f, 0, 1);

			v = VectorToArray(&m_spawn_mat.m_ambient_color);
			ImGui::DragFloat4("Ambient Color", v.setArray(), 0.01f, 0.0f, 1.0f);

			v = VectorToArray(&m_spawn_mat.m_diffuse_color);
			ImGui::DragFloat4("Diffuse Color", v.setArray(), 0.01f, 0.0f, 1.0f);

			v = VectorToArray(&m_spawn_mat.m_specular_color);
			ImGui::DragFloat4("Specular Color", v.setArray(), 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Shininess", &m_spawn_mat.m_shininess, 0.05f, 0, 40);

			v = VectorToArray(&m_spawn_mat.m_rim_color);
			ImGui::DragFloat4("RimLight Color", v.setArray(), 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("RimLight Amount", &m_spawn_mat.m_rim_power, 0.05f, 0, 40);

			m_cubes[m_cube_ID]->setMaterial(m_spawn_mat);

			if (ImGui::Button("Accept")) m_show_mat_window = false;

			ImGui::End();
		}

		if (ImGui::Button("Accept")) m_show_obj_window = false;

		ImGui::SetNextWindowSize(ImVec2(300, 200));
		ImGui::SetNextWindowPos(ImVec2(0, 450));
		ImGui::Begin("Primitive Textures");

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

		ImGui::End();

		ImGui::End();
	}

	ImGui::SetNextWindowSize(ImVec2(100, 200));
	ImGui::SetNextWindowPos(ImVec2(0, 250));
	ImGui::Begin("Scene Objects");

	for (int i = 0; i < m_cubes.size(); i++)
	{
		std::string name = "Cube " + std::to_string(i);
		if (ImGui::Button(name.c_str()))
		{
			m_show_obj_window = true;
			m_spawn_pos = m_cubes[i]->getPosition();
			m_spawn_scale = m_cubes[i]->getScale();
			m_spawn_rot = m_cubes[i]->getRotation();
			m_cube_ID = i;
		}
	}

	ImGui::End();
}

void WorldObjectManager::render()
{
	for (int i = 0; i < m_cubes.size(); i++)
	{
		m_cubes[i]->render(-1, true);
	}
}

Vector3D WorldObjectManager::CubeAABBCollision(Vector3D old_pos, Vector3D new_pos, Vector3D size)
{
	Vector3D move = new_pos - old_pos;
	//bool zmove = 1 * (move.m_z > 0) + -1 * (move.m_z < 0);
	//bool ymove = 1 * (move.m_y > 0) + -1 * (move.m_y < 0);
	//bool xmove = 1 * (move.m_x > 0) + -1 * (move.m_x < 0);

	Vector3D halfsize = size / 2.0f;
	Vector3D pos2;
	Vector3D halfsize2;

	for (int i = 0; i < m_cubes.size(); i++)
	{
		//pos2 = m_cubes[i]->getPosition();
		//halfsize2 = m_cubes[i]->getScale() / 2.0f;

		////立方体同士の当たり判定
		//Vector3D dif = pos2 - new_pos;
		//if (abs(dif.m_x) > (halfsize.m_x + halfsize2.m_x)) continue;
		//if (abs(dif.m_y) > (halfsize.m_y + halfsize2.m_y)) continue;
		//if (abs(dif.m_z) > (halfsize.m_z + halfsize2.m_z)) continue;

		pos2 = m_cubes[i]->getPosition();
		halfsize2 = m_cubes[i]->getScale() / 2.0f;

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









		//X collision resolution
		//if ((dif.m_x < -(halfsize.m_x + halfsize2.m_x)) || (dif.m_x > (halfsize.m_x + halfsize2.m_x)))

		//当たりの解決
		//Vector3D result = new_pos;
		//float dot = -1.0f;
		//float newdot = -10.0f;
		//int choice = -1;
		//dif.normalize();

		//if (xmove)
		//{
		//	//right edge
		//	if (result.m_x + halfsize.m_x > pos2.m_x - halfsize2.m_x && xmove == 1)
		//	{
		//		result.m_x = pos2.m_x - (halfsize.m_x + halfsize2.m_x);
		//	}
		//	//left edge
		//	else if (result.m_x - halfsize.m_x < pos2.m_x + halfsize2.m_x && xmove == -1)
		//	{
		//		result.m_x = pos2.m_x + (halfsize.m_x + halfsize2.m_x);
		//	}
		//}

		//if (ymove)
		//{
		//	//top edge
		//	if (result.m_y + halfsize.m_y > pos2.m_y - halfsize2.m_y && xmove == 1)
		//	{
		//		result.m_y = pos2.m_y - (halfsize.m_y + halfsize2.m_y);
		//	}
		//	//bottom edge
		//	else if (result.m_y - halfsize.m_y < pos2.m_y + halfsize2.m_y && xmove == -1)
		//	{
		//		result.m_y = pos2.m_y + (halfsize.m_y + halfsize2.m_y);
		//	}
		//}

		//if (zmove)
		//{
		//	//right edge
		//	if (result.m_z + halfsize.m_z > pos2.m_z - halfsize2.m_z && xmove == 1)
		//	{
		//		result.m_z = pos2.m_z - (halfsize.m_z + halfsize2.m_z);
		//	}
		//	//left edge
		//	else if (result.m_z - halfsize.m_z < pos2.m_z + halfsize2.m_z && xmove == -1)
		//	{
		//		result.m_z = pos2.m_z + (halfsize.m_z + halfsize2.m_z);
		//	}
		//}


		////上の面
		//newdot = Vector3D::dot(dif, Vector3D(0, -1, 0));
		//if (newdot > dot)
		//{
		//	dot = newdot;
		//	choice = 0;
		//}

		////下の面
		//newdot = Vector3D::dot(dif, Vector3D(0, 1, 0));
		//if (newdot > dot)
		//{
		//	dot = newdot;
		//	choice = 1;
		//}

		////右の面
		//newdot = Vector3D::dot(dif, Vector3D(-1, 0, 0));
		//if (newdot > dot)
		//{
		//	dot = newdot;
		//	choice = 2;
		//}

		////左の面
		//newdot = Vector3D::dot(dif, Vector3D(1, 0, 0));
		//if (newdot > dot)
		//{
		//	dot = newdot;
		//	choice = 3;
		//}

		////後ろの面
		//newdot = Vector3D::dot(dif, Vector3D(0, 0, -1));
		//if (newdot > dot)
		//{
		//	dot = newdot;
		//	choice = 4;
		//}

		////前の面
		//newdot = Vector3D::dot(dif, Vector3D(0, 0, 1));
		//if (newdot > dot)
		//{
		//	dot = newdot;
		//	choice = 5;
		//}

		//assert(choice >= 0, "A collision was detected but unresolved");

		//switch (choice)
		//{
		//case 0: //top face adjustment
		//	result.m_y = pos2.m_y + (halfsize.m_y + halfsize2.m_y);
		//	break;
		//case 1: //bottom face adjustment
		//	result.m_y = pos2.m_y - (halfsize.m_y + halfsize2.m_y);
		//	break;
		//case 2: //right face adjustment
		//	result.m_x = pos2.m_x + (halfsize.m_x + halfsize2.m_x);
		//	break;
		//case 3: //left face adjustment
		//	result.m_x = pos2.m_x - (halfsize.m_x + halfsize2.m_x);
		//	break;
		//case 4: //back face adjustment
		//	result.m_z = pos2.m_z + (halfsize.m_z + halfsize2.m_z);
		//	break;
		//case 5: //forward face adjustment
		//	result.m_z = pos2.m_z - (halfsize.m_z + halfsize2.m_z);
		//	break;
		//}

		return result;
	}

	return new_pos;
}
