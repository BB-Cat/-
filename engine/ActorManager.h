#pragma once
#include "Prerequisites.h"
#include "Actor.h"
#include "Player.h"
#include "Enemy.h"
#include "Powerup.h"
#include "SkinnedMesh.h"
#include <vector>

class Faction
{
public:
	Faction(std::wstring name) : m_name(name) {}
private:
	std::wstring m_name;
	std::vector<std::shared_ptr<Actor>> m_actors;
private:
	friend class ActorManager;
};

class ActorManager
{
private:
	ActorManager();
	static ActorManager* instance;

	enum FactionID
	{
		PLAYER,
		ENEMY,
		AMBIENT,
	};
public:
	static ActorManager* get();
	~ActorManager();

	//�S�ẴA�N�^�[�̍X�V�֐����s��
	void updateAll(float delta);
	//��̃J�e�S���[�̃A�N�^�[�̍X�V���s��
	void updateFaction(std::wstring name, float delta);
	//�A�N�^�[�̃V�F�[�_�[���g���ă��b�V����`�悷��
	void renderAll(float delta);
	//��̃J�e�S���[�̃A�N�^�[�̕`����s��
	void renderFaction(std::wstring name, float delta);
	//�e���ł��郁�b�V�����V���h�[�}�b�v�ɕ`�悷��
	void renderShadowsAll(float delta);
	//��̃J�e�S���[�̃A�N�^�[�̉e��`�悷��
	void renderFactionShadows(std::wstring name, float delta);

public:
	//���݋N����Ԃ��Ă���v���C���[�I�u�W�F�N�g�̈ʒu�����擾
	Vector3D getActivePlayerPosition();
	//���݋N����Ԃ��Ă���v���C���[�I�u�W�F�N�g�̉�]�����擾
	Vector3D getActivePlayerDirection();

public:
	//���݋N����Ԃ��Ă���v���C���[�I�u�W�F�N�g��imgui�E�C���h�[��`�悷��
	void activePlayerImGui();
public:
	Faction* getFaction(std::wstring faction_name);

private:
	std::vector<Faction> m_actor_lists;
	const float m_active_player = 0;
};