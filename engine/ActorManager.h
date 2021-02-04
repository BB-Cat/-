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
	~Faction() 
	{ 
		m_actors.clear();
	}
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

	static void release();

	//�}���`�X���b�h�ŏ����������v���C���[�C���X�^���X���}�l�[�W���[�ɓn��
	void setPlayer(std::shared_ptr<Player> player);

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
	int getActivePlayerState();
	//���݋N����Ԃ��Ă���v���C���[�I�u�W�F�N�g�̈ʒu�����擾
	Vec3 getActivePlayerPosition();
	//�N����Ԃ̃v���C���[�I�u�W�F�N�g�̈ʒu��ݒ肷��
	void setActivePlayerPosition(Vec3 pos);
	//�N����Ԃ̃v���C���[�I�u�W�F�N�g�̏㏸���~�߂�
	void stopActivePlayerAscent();
	//�N����Ԃ̃v���C���[�I�u�W�F�N�g�̒��n��������
	void stopActivePlayerJump();
	//�N����Ԃ̃v���C���[�I�u�W�F�N�g�̒��n��������
	void startActivePlayerFall();
	//���݋N����Ԃ��Ă���v���C���[�I�u�W�F�N�g�̉�]�����擾
	Vec3 getActivePlayerDirection();

	bool isNoPlayer() { return m_player == nullptr; }

public:
	//���݋N����Ԃ��Ă���v���C���[�I�u�W�F�N�g��imgui�E�C���h�[��`�悷��
	void activePlayerImGui();
public:
	Faction* getFaction(std::wstring faction_name);

private:
	std::vector<Faction> m_actor_lists;
	const float m_active_player = 0;
	PlayerPtr m_player = nullptr;
};