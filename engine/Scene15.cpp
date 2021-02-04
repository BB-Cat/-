#include "Scene15.h"
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

Scene15::Scene15(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::FREE);
	CameraManager::get()->setCamPos(Vec3(2.29f, 2.54f, -3.66f));
	CameraManager::get()->setCamRot(Vec2(0.254f, -0.797f));
	CameraManager::get()->setSpeed(0.2f);

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.2, 0.2, 0.4));

	m_global_light_rotation = Vec2(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(0.4, 0.6, 0.0);
	m_ambient_light_color = Vec3(0.4, 0.3, 0.4);

	m_ground = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\Floor\\floor.fbx", true, nullptr);
	marker = PrimitiveGenerator::get()->createUnitSphere(nullptr, nullptr, nullptr, nullptr);

	m_model = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\CharacterRough\\lp.fbx", true, nullptr, D3D11_CULL_BACK);
	m_model->setAnimationCategory(Animation::Type::Player);
	//m_model->loadAnimation(nullptr, Animation::Player::Idle, L"..\\Assets\\CharacterRough\\lp_idle.fbx", true, true, 0, false, false);
	//m_model->loadAnimation(nullptr, Animation::Player::Idle2, L"..\\Assets\\CharacterRough\\lp_idle2.fbx", true, true, 0, false, false);
	//m_model->loadAnimation(nullptr, Animation::Player::Idle3, L"..\\Assets\\CharacterRough\\lp_idle3.fbx", true, true, 0, false, false);

	//m_model->loadAnimation(nullptr, Animation::Player::Walk, L"..\\Assets\\CharacterRough\\lp_walk.fbx");
	//m_model->loadAnimation(nullptr, Animation::Player::WalkBackward, L"..\\Assets\\CharacterRough\\lp_walkback.fbx");
	//m_model->loadAnimation(nullptr, Animation::Player::Run, L"..\\Assets\\CharacterRough\\lp_run.fbx");
	//m_model->loadAnimation(nullptr, Animation::Player::Stop, L"..\\Assets\\CharacterRough\\lp_stop.fbx");
	//m_model->loadAnimation(nullptr, Animation::Player::Jump, L"..\\Assets\\CharacterRough\\lp_jump.fbx", false, true, 0, true, 0.99f);
	//m_model->loadAnimation(nullptr, Animation::Player::LandToIdle, L"..\\Assets\\CharacterRough\\lp_land.fbx", false, true);
	//m_model->loadAnimation(nullptr, Animation::Player::LandHard, L"..\\Assets\\CharacterRough\\lp_land_hard.fbx", false, true);
	//m_model->loadAnimation(nullptr, Animation::Player::LandToRun, L"..\\Assets\\CharacterRough\\lp_land_to_run.fbx", false, true);
	//m_model->loadAnimation(nullptr, Animation::Player::Roll, L"..\\Assets\\CharacterRough\\lp_roll.fbx", false, true);
	//m_model->loadAnimation(nullptr, Animation::Player::DodgeBack, L"..\\Assets\\CharacterRough\\lp_dodge_back.fbx", false, true, 0.8f);
	m_model->loadAnimation(nullptr, Animation::Player::Attack1, L"..\\Assets\\CharacterRough\\lp_slash_downward.fbx", true, true);
	m_model->loadAnimation(nullptr, Animation::Player::Attack2, L"..\\Assets\\CharacterRough\\lp_slash_horizontal.fbx", true, true);
	m_model->loadAnimation(nullptr, Animation::Player::Attack3, L"..\\Assets\\CharacterRough\\lp_slash_backhand.fbx", true, true);
	m_model->loadAnimation(nullptr, Animation::Player::Attack4, L"..\\Assets\\CharacterRough\\lp_slash_roundabout.fbx", true, true);
	m_model->loadAnimation(nullptr, Animation::Player::Attack5, L"..\\Assets\\CharacterRough\\lp_running_slash.fbx", true, true);

	//m_model->setBlendAnimation(Animation::Player::Attack1);
	//アニメーションの初期設定
	m_model->setAnimation(-1);
	m_model->setAnimation(-1);
}

Scene15::~Scene15()
{

}

void Scene15::update(float delta)
{
	CameraManager::get()->update(delta);
	m_timer++;

	if (m_is_play && m_model->getActiveAnimation() != nullptr)
	{
		m_model->getActiveAnimation()->update(delta * m_play_speed);
		m_highlighted_frame = m_model->getActiveAnimation()->getFrame();
	}
}

void Scene15::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(250, 700));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Animation List");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	if (ImGui::Button("Show Explanation", ImVec2(200, 30))) m_first_time = true;
	//ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);

	ImGui::NewLine();
	ImGui::NewLine();

	ImVec2 buttonsize = ImVec2(200, 20);
	if (ImGui::Button("Attack1", buttonsize))
	{
		m_model->setAnimation(Animation::Player::Attack1);
		m_anm = m_model->getActiveAnimation();
		m_highlighted_frame = 0;
		m_highlighted_hitbox = -1;
	}
	if (ImGui::Button("Attack2", buttonsize))
	{
		m_model->setAnimation(Animation::Player::Attack2);
		m_anm = m_model->getActiveAnimation();
		m_highlighted_frame = 0;
		m_highlighted_hitbox = -1;
	}
	if (ImGui::Button("Attack3", buttonsize))
	{
		m_model->setAnimation(Animation::Player::Attack3);
		m_anm = m_model->getActiveAnimation();
		m_highlighted_frame = 0;
		m_highlighted_hitbox = -1;
	}
	if (ImGui::Button("Attack4", buttonsize))
	{
		m_model->setAnimation(Animation::Player::Attack4);
		m_anm = m_model->getActiveAnimation();
		m_highlighted_frame = 0;
		m_highlighted_hitbox = -1;
	}
	if (ImGui::Button("Attack5", buttonsize))
	{
		m_model->setAnimation(Animation::Player::Attack5);
		m_anm = m_model->getActiveAnimation();
		m_highlighted_frame = 0;
		m_highlighted_hitbox = -1;
	}
	if (ImGui::Button("Stop", buttonsize))
	{
		m_model->setAnimation(-1);
		m_anm = m_model->getActiveAnimation();
		m_highlighted_frame = 0;
		m_highlighted_hitbox = -1;
	}

	ImGui::Text("Frame");
	int maxframes = m_model->getAnimationFrameCount();
	if (ImGui::SliderInt("2", &m_highlighted_frame, 0, maxframes - 1, nullptr, 1.0f))
	{
		m_model->setAnimationFrame(m_highlighted_frame);
	}
	if (ImGui::Button("Play Animation", buttonsize)) m_is_play = !m_is_play;

	ImGui::Text("Play Speed");
	ImGui::SliderFloat("3", &m_play_speed, 1.0f, 2.0f);

	if (m_anm != nullptr)
	{

		if (ImGui::CollapsingHeader("Animation Editor"))
		{
			hb_vec = m_anm->getHitboxes();

			if (ImGui::Button("Add Capsule Collider", buttonsize))
			{
				float frames[] = {m_highlighted_frame, m_highlighted_frame };
				float damage = 1;

				std::shared_ptr<Collider> p = std::shared_ptr<Collider>(new CapsuleCollider(0.5f, 1.0f));

				m_anm->addHitbox(std::shared_ptr<AnmHitbox>(new AnmHitbox(frames, p, damage)));
				m_highlighted_hitbox = hb_vec.size() - 1;
			}

			for (int i = 0; i < hb_vec.size(); i++)
			{
				std::string s;
				s += 
					std::to_string((int)hb_vec[i]->m_active_frames[0]) +
					" - " +
					std::to_string((int)hb_vec[i]->m_active_frames[1]);

				ImGui::PushID(std::to_string(i).c_str());
				
				if (ImGui::Button(s.c_str(), buttonsize))
				{
					m_highlighted_frame = hb_vec[i]->m_active_frames[0];
					m_highlighted_hitbox = i;
				}

				ImGui::PopID();
			}
		}

	}

	if (m_highlighted_hitbox > hb_vec.size() - 1) m_highlighted_hitbox = -1;

	if (m_highlighted_hitbox >= 0)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 250));
		ImGui::SetNextWindowPos(ImVec2(700, 20));

		ImGui::Begin("Hitbox");


		CapsuleCollider* c = reinterpret_cast<CapsuleCollider*>(hb_vec[m_highlighted_hitbox]->m_collider.get());
		Vec3 rot = c->getRotation();
		VectorToArray v = VectorToArray(&rot);
		if (ImGui::DragFloat3("rotation", v.setArray(), 0.01f))
		{
			c->setRotation(rot);
		}
		Vec3 pos = c->getOffset();
		v = VectorToArray(&pos);
		if (ImGui::DragFloat3("offset", v.setArray(), 0.01f))
		{
			c->setOffset(pos);
		}

		float len = c->getCoreLength();
		if (ImGui::DragFloat("core length", &len, 0.01f))
		{
			c->setCoreLength(len);
		}

		float r = c->getRadius();
		if (ImGui::DragFloat("r1", &r, 0.01f))
		{
			c->setRadius(r);
		}

		ImGui::DragInt2("active frames", &hb_vec[m_highlighted_hitbox]->m_active_frames[0], 0.01f);
		if (ImGui::DragInt("damage", &hb_vec[m_highlighted_hitbox]->m_damage_value, 0.5f))
		{
			int a = 3;
		}

		ImGui::End();
	}

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));

		ImGui::OpenPopup("Animation Test Popup");
		ImGui::BeginPopupModal("Animation Test Popup");

		ImGui::TextWrapped("This scene is for making hitbox data frame by frame for attacks.  Choose an animation, then 'Add capsule collider,' and click on the new button.  You can adjust the hitbox's location, rotation, active frames, and more. ");

		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}


	ImGui::End();
}

void Scene15::renderCapsule(Collider* capsule)
{
	PrimitivePtr shape;
	Vec3 s;
	Vec3 p;
	Vec3 r;

	CapsuleCollider* c1 = nullptr;
	c1 = reinterpret_cast<CapsuleCollider*>(capsule);

	r = c1->getRotation();
	p = m_modelpos + c1->getOffset();
	s = Vec3(1, 1, 1);

	shape = PrimitiveGenerator::get()->createCustomCapsule(c1->getRadius(), 
		c1->getCoreLength(), 15, 15, nullptr, nullptr, nullptr, nullptr);

	Material_Obj m = shape->getMaterial();
	//save the original transparency of the object
	Vector4D temp = m.m_diffuse_color;
	//reduce the transparency before rendering
	m.m_diffuse_color = Vector4D(0.8f, 0.8f, 0.6f, 0.35f);
	m.m_transparency = 0.3f;
	shape->setMaterial(m);

	shape->render(s, p, r, Shaders::LAMBERT_RIMLIGHT);

}

void Scene15::shadowRenderPass(float delta)
{
}

void Scene15::mainRenderPass(float delta)
{

	m_model->renderMesh(delta, Vec3(1, 1, 1), m_modelpos, Vec3(0, 180 * 0.01745f, 0), Shaders::LAMBERT_RIMLIGHT, false, 0);


	marker->setColor(Vec3(1.0, 1.0, 0.2));
	marker->setTransparency(1.0f);
	marker->render(Vec3(0.2), Vec3(0, 0, 0), Vec3(), LAMBERT, false);

	m_ground->renderMesh(delta, Vec3(0.5f), Vec3(0, 0, 0), Vec3(0, 0, 0), DEBUG_GRID);

	for (int i = 0; i < hb_vec.size(); i++)
	{
		if (!hb_vec[i]->isActiveAtFrame(m_highlighted_frame)) continue;
		renderCapsule(hb_vec[i]->m_collider.get());
	}
}