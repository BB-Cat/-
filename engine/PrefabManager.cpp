#include "PrefabManager.h"
#include "GraphicsEngine.h"
#include "SkinnedMeshManager.h"
#include "VectorToArray.h"
#include "SkinnedMesh.h"
#include "PrimitiveGenerator.h"
#include "Primitive.h"
#include "Blend.h"

PrefabManager* PrefabManager::instance = nullptr;

PrefabManager::PrefabManager()
{
}

PrefabManager::~PrefabManager()
{
}

bool PrefabManager::createPrefab(std::wstring file, std::string name)
{
	std::wstring full_file = L"..\\Assets\\PrefabMeshes\\Meshes\\" + file;
	const wchar_t* tempwchar = full_file.c_str();

	PrefabMesh p;
	p.mesh = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(tempwchar,
		true, nullptr, D3D11_CULL_BACK);
	if (p.mesh == nullptr) return false;

	m_prefabs.push_back(p);
	m_prefab_names.push_back(name);
	m_prefab_files.push_back(file);
	return true;
}

PrefabMesh* PrefabManager::findPrefab(std::string name)
{
	for (int i = 0; i < m_prefabs.size(); i++)
	{
		if (m_prefab_names[i] == name)
		{
			return &m_prefabs[i];
		}
	}

	return nullptr;
}

bool PrefabManager::ImGuiModifyPrefabs()
{
	//ImGui::SetNextWindowSize(ImVec2(600, 250));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::SetNextWindowPos(ImVec2(0, 65));
	ImGui::Begin("Return Button", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

	if (ImGui::Button("Return", ImVec2(200.0f, 30.0f)))
	{
		ImGui::End();
		return true;
	}

	ImGui::End();

	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::SetNextWindowPos(ImVec2(0, 110));
	ImGui::SetNextWindowSize(ImVec2(150, 300));
	ImGui::Begin("Prefabs");

	for (int i = 0; i < m_prefabs.size(); i++)
	{
		if (ImGui::Button(m_prefab_names[i].c_str())) m_focused_prefab = i;
	}

	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(250, 600));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::SetNextWindowPos(ImVec2(755, 20));
	ImGui::Begin("Settings");


	if (m_focused_prefab >= 0)
	{
		VectorToArray v = VectorToArray(&Vector4D());
		if (ImGui::CollapsingHeader("General"))
		{
			ImGui::Text(("Prefab Name: " + m_prefab_names[m_focused_prefab]).c_str());
			if (ImGui::InputText("Name", m_name, ARRAYSIZE(m_name), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				m_prefab_names[m_focused_prefab] = m_name;
				memset(m_name, 0, sizeof(m_name));
			}

			v = VectorToArray(&m_prefabs[m_focused_prefab].default_scale);
			ImGui::DragFloat3("Default Scale", v.setArray(), 0.05f, -100.0f, 100.0f);
			v = VectorToArray(&m_prefabs[m_focused_prefab].default_rot);
			ImGui::DragFloat3("Default Rotation", v.setArray(), 0.05f, -100.0f, 100.0f);
		}
		//material
		if (ImGui::CollapsingHeader("Material"))
		{
			m_focused_material = m_prefabs[m_focused_prefab].mesh->getMaterial();

			v = VectorToArray(&m_focused_material.m_diffuse_color);
			ImGui::DragFloat3("Diffuse Color", v.setArray(), 0.01f, 0.0f, 1.0f);

			v = VectorToArray(&m_focused_material.m_specular_color);
			ImGui::DragFloat3("Specular Color", v.setArray(), 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Shininess", &m_focused_material.m_shininess, 0.05f, 0, 40);

			v = VectorToArray(&m_focused_material.m_rim_color);
			ImGui::DragFloat3("RimLight Color", v.setArray(), 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("RimLight Amount", &m_focused_material.m_rim_power, 0.05f, 0, 40);

			m_prefabs[m_focused_prefab].mesh->setMaterial(m_focused_material);
		}
		//collider
		if (ImGui::CollapsingHeader("Collider"))
		{
			bool focused_has_collider = (m_prefabs[m_focused_prefab].collider != nullptr);
			if (ImGui::Checkbox("Enable AABB Collider", &focused_has_collider))
			{
				if (!focused_has_collider)
				{
					delete m_prefabs[m_focused_prefab].collider;
					focused_has_collider = false;
				}
				else
				{
					Collider* temp = new CubeCollider(Vector3D(1, 1, 1));
					m_prefabs[m_focused_prefab].collider = temp;
					focused_has_collider = true;
					m_show_collider = true;
				}
			}
			if (focused_has_collider)
			{
				ImGui::Checkbox("Show Collider", &m_show_collider);
				if (m_show_collider)
				{
					ImGui::PushID("Collider");
					Vector3D bounding_box = m_prefabs[m_focused_prefab].collider->getBoundingBox();
					v = VectorToArray(&bounding_box);
					ImGui::DragFloat3("Size", v.setArray(), 0.025f, 0.01);
					m_prefabs[m_focused_prefab].collider->setBoundingBox(bounding_box);

					Vector3D offset = m_prefabs[m_focused_prefab].collider->getOffset();
					v = VectorToArray(&offset);
					ImGui::DragFloat3("Offset", v.setArray(), 0.025f, 0.01);
					m_prefabs[m_focused_prefab].collider->setOffset(offset);
					ImGui::PopID();
				}
			}
		}
		//shader
	}
	ImGui::End();

	return false;
}

void PrefabManager::renderEditingPrefab()
{
	if (m_focused_prefab >= 0 && m_focused_prefab < m_prefabs.size())
	{
		PrefabMesh *m = &m_prefabs[m_focused_prefab];
		m->mesh->renderMesh(0, m->default_scale, Vector3D(0, 0, 0), m->default_rot, m->default_shader);

		if (m_show_collider) renderEditingBoundingBox();
	}
}

void PrefabManager::renderEditingBoundingBox()
{
	//create a cube to render bounding boxes.  temporary
	PrimitivePtr cube = PrimitiveGenerator::get()->createCube(nullptr, nullptr, nullptr, nullptr);

	//GraphicsEngine::get()->getShaderManager()->setPipeline(Shaders::FLAT);

	Material_Obj m = cube->getMaterial();
	//save the original transparency of the object
	Vector4D temp = m.m_diffuse_color;
	//reduce the transparency before rendering
	m.m_diffuse_color = Vector4D(0.4f, 0.2f, 0.2f, 1.0f);
	cube->setMaterial(m);

	//set the blend to screen
	BlendMode::get()->SetBlend(SCREEN);

	Collider* c = m_prefabs[m_focused_prefab].collider;
	if (c == nullptr) return;

	Vector3D s = c->getBoundingBox();
	Vector3D p = c->getOffset();
	Vector3D r = {};

	cube->render(s, p, r, Shaders::LAMBERT);

	//return settings to how they were
	BlendMode::get()->SetBlend(ALPHA);
}
