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

	//マルチスレッドで初期化したプレイヤーインスタンスをマネージャーに渡す
	void setPlayer(std::shared_ptr<Player> player);

	//全てのアクターの更新関数を行う
	void updateAll(float delta);
	//一つのカテゴリーのアクターの更新を行う
	void updateFaction(std::wstring name, float delta);
	//アクターのシェーダーを使ってメッシュを描画する
	void renderAll(float delta);
	//一つのカテゴリーのアクターの描画を行う
	void renderFaction(std::wstring name, float delta);
	//影ができるメッシュをシャドーマップに描画する
	void renderShadowsAll(float delta);
	//一つのカテゴリーのアクターの影を描画する
	void renderFactionShadows(std::wstring name, float delta);

public:
	//現在起動状態しているプレイヤーオブジェクトの位置情報を取得
	int getActivePlayerState();
	//現在起動状態しているプレイヤーオブジェクトの位置情報を取得
	Vec3 getActivePlayerPosition();
	//起動状態のプレイヤーオブジェクトの位置を設定する
	void setActivePlayerPosition(Vec3 pos);
	//起動状態のプレイヤーオブジェクトの上昇を止める
	void stopActivePlayerAscent();
	//起動状態のプレイヤーオブジェクトの着地をさせる
	void stopActivePlayerJump();
	//起動状態のプレイヤーオブジェクトの着地をさせる
	void startActivePlayerFall();
	//現在起動状態しているプレイヤーオブジェクトの回転情報を取得
	Vec3 getActivePlayerDirection();

	bool isNoPlayer() { return m_player == nullptr; }

public:
	//現在起動状態しているプレイヤーオブジェクトのimguiウインドーを描画する
	void activePlayerImGui();
public:
	Faction* getFaction(std::wstring faction_name);

private:
	std::vector<Faction> m_actor_lists;
	const float m_active_player = 0;
	PlayerPtr m_player = nullptr;
};