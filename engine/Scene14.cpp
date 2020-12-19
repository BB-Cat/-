#include "Scene14.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "ConstantBufferSystem.h"
#include "Sprite.h"
#include "AnmEnumeration.h"
#include "Texture3D.h"
#include "DeviceContext.h"
#include "Texture.h"
#include "WorldObject.h"
#include "WorldObjectManager.h"
#include "ActorManager.h"
#include "Primitive.h"
#include "Collision.h"

#include <iostream>
#include <fstream>  



//bool Scene03::m_first_time = true;

Scene14::Scene14(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::FREE);
	CameraManager::get()->setCamPos(Vec3(0, 0, -5));

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.2, 0.2, 0.4));

	m_global_light_rotation = Vec2(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(0.4, 0.6, 0.0);
	m_ambient_light_color = Vec3(0.4, 0.3, 0.4);



	//testing capsules
	r1 = 0.25f;
	r2 = 0.25f;
	len1 = 0.5f;
	len2 = 0.5f;
	rot1 = Vec2(0.0f, 0.0f);
	rot2 = Vec2(0.0f, 0.0f);

	obj1 = new CapsuleCollider(r1, len1, rot1);
	obj2 = new CapsuleCollider(r2, len2, rot2);



	marker = PrimitiveGenerator::get()->createUnitSphere(nullptr, nullptr, nullptr, nullptr);
	pos1 = Vec3(3, 0, 3);
	prevpos1 = pos1;

	start1 = Vec3(0, 0.0f, -5);
	start2 = Vec3(4.15f, 0, 0);
}

Scene14::~Scene14()
{
	delete obj1;
	delete obj2;
}

void Scene14::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta, width, height);

	m_scene_light_dir = Vec3(sinf(m_global_light_rotation.x), m_global_light_rotation.y, cosf(m_global_light_rotation.x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);


	m_timer++;
}

void Scene14::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, 65));
	ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	prevpos1 = pos1;
	VectorToArray v = VectorToArray(&pos1);
	ImGui::DragFloat3("Pos1", v.setArray(), 0.01f);
	v = VectorToArray(&pos2);
	ImGui::DragFloat3("Pos2", v.setArray(), 0.01f);

	ImGui::SliderFloat("Mass Balance", &mass, 0, 1.0f);

	v = VectorToArray(&movement);
	ImGui::DragFloat3("Obj1 Move Direction", v.setArray(), 0.001f, -1.0f, 1.0f);

	if (ImGui::Button("Reset Collision"))
	{
		pos1 = start1;
		pos2 = start2;
	}

	if (ImGui::Button("Reset Camera"))
	{
		CameraManager::get()->setCamPos(Vec3(0, 0, -5));
		CameraManager::get()->setCamRot(Vec2(0, 0));
	}

	if (ImGui::Button("Toggle Collision Test")) is_simulate = !is_simulate;

	v = VectorToArray(&start1);
	ImGui::DragFloat3("Start1", v.setArray(), 0.01f);
	v = VectorToArray(&start2);
	ImGui::DragFloat3("Start2", v.setArray(), 0.01f);

	if (ImGui::CollapsingHeader("Capsule Settings"))
	{
		v = VectorToArray(&rot1);
		if (ImGui::DragFloat3("rot1", v.setArray(), 0.01f))
		{
			CapsuleCollider* temp = reinterpret_cast<CapsuleCollider*>(obj1);
			temp->setRotation(rot1);
		}
		v = VectorToArray(&rot2);
		if (ImGui::DragFloat3("rot2", v.setArray(), 0.01f))
		{
			CapsuleCollider* temp = reinterpret_cast<CapsuleCollider*>(obj2);
			temp->setRotation(rot2);
		}
		if(ImGui::DragFloat("len1", &len1, 0.01f))
		{
			CapsuleCollider* temp = reinterpret_cast<CapsuleCollider*>(obj1);
			temp->setCoreLength(len1);
		}
		if (ImGui::DragFloat("len2", &len2, 0.01f))
		{
			CapsuleCollider* temp = reinterpret_cast<CapsuleCollider*>(obj2);
			temp->setCoreLength(len2);
		}
		if (ImGui::DragFloat("r1", &r1, 0.01f))
		{
			CapsuleCollider* temp = reinterpret_cast<CapsuleCollider*>(obj1);
			temp->setRadius(r1);
		}
		if(ImGui::DragFloat("r2", &r2, 0.01f))
		{
			CapsuleCollider* temp = reinterpret_cast<CapsuleCollider*>(obj2);
			temp->setRadius(r2);
		}

		if (ImGui::Button("Show Bounding Box")) show_bbs = !show_bbs;
	}

	ImGui::End();
}

void Scene14::renderCollider(Vec3 pos, Collider* c, Vec3 col, int type, float radius, float len)
{
	PrimitivePtr shape;
	Vec3 s;
	Vec3 p;
	Vec3 r = {};
	CapsuleCollider* c1 = nullptr;

	switch (type)
	{
	case ColliderTypes::Cube: shape = PrimitiveGenerator::get()->createUnitCube(nullptr, nullptr, nullptr, nullptr);
		s = c->getBoundingBox();
		break;
	case ColliderTypes::Sphere: shape = PrimitiveGenerator::get()->createUnitSphere(nullptr, nullptr, nullptr, nullptr);
		s = c->getBoundingBox();
		break;
	case ColliderTypes::Capsule: 
		shape = PrimitiveGenerator::get()->createCustomCapsule(radius, len, 30, 30,  nullptr, nullptr, nullptr, nullptr);
		c1 = reinterpret_cast<CapsuleCollider*>(c);
		r = c1->getRotation();
		s = Vec3(1,1,1);
		break;
	}

	Material_Obj m = shape->getMaterial();
	//save the original transparency of the object
	Vector4D temp = m.m_diffuse_color;
	//reduce the transparency before rendering
	m.m_diffuse_color = Vector4D(col.x, col.y, col.z, 0.35f);
	m.m_transparency = 0.3f;
	shape->setMaterial(m);




	p = pos + c->getOffset();

	shape->render(s, p, r, Shaders::LAMBERT_RIMLIGHT);

}

void Scene14::shadowRenderPass(float delta)
{
}

void Scene14::mainRenderPass(float delta)
{
	Vec3 col = Vec3(0.5, 0.5, 0.75);
	//if (Collision::DetectCollision(obj1, pos1, obj2, pos2))
	//{
	//	col = Vec3(0.75, 0.5, 0.5);
	//}

	if (is_simulate)
	{
		pos1 += movement;
	}
	if (Collision::ResolveCollision(obj1, pos1, prevpos1, obj2, pos2, mass))
	{
		col = Vec3(0.75, 0.5, 0.5);
	}

	//if (Collision::DetectCollision(obj1, pos1, obj2, pos2))
	//{
	//	col = Vec3(0.75, 0.5, 0.5);
	//}

	//if (Collision::DetectCapsuleCapsule(reinterpret_cast<CapsuleCollider*>(obj1), pos1, reinterpret_cast<CapsuleCollider*>(obj2), pos2, &markerpos, &marker2pos))
	//{
	//	col = Vec3(0.75, 0.5, 0.5);
	//}

	renderCollider(pos1, obj1, col, ColliderTypes::Capsule, r1, len1);
	renderCollider(pos2, obj2, col, ColliderTypes::Capsule, r2, len2);

	if (show_bbs)
	{
		renderCollider(pos1, obj1, col, ColliderTypes::Cube, r1, len1);
		renderCollider(pos2, obj2, col, ColliderTypes::Cube, r2, len2);
	}

	//reinterpret_cast<CapsuleCollider*>(obj1)->getCaps(markerpos, marker2pos);

	marker->setColor(Vec3(1.0, 1.0, 0.2));
	marker->setTransparency(0.3f);
	marker->render(Vec3(0.1), markerpos, Vec3(), LAMBERT, false);
	marker->render(Vec3(0.1), marker2pos, Vec3(), LAMBERT, false);
}