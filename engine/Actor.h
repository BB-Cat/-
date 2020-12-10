#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include "SkinnedMesh.h"

class Actor
{
public:
	Actor(bool has_shadow = false) : m_has_shadow(has_shadow) {}

	Vec3 getPosition() { return m_pos; };
	Vec3 getDirectionVector() { return Vec3(sinf(m_angle * 0.01745f), 0, cosf(m_angle * 0.01745f)); };
	Vec3 getRightVector() { return Vec3(sinf((m_angle + 90.0f) * 0.01745f), 0, cosf((m_angle + 90.0f) * 0.01745f)); };

	virtual void update(float delta) = 0;
	virtual void render(float delta) = 0;
	virtual void renderShadow(float delta) = 0;

	virtual void imGuiWindow() {}
	//virtual void animationTree() {}
protected:
	Vec3 m_pos;
	Vec3 m_rot;
	Vec3 m_scale;
	float m_angle;
	SkinnedMeshPtr m_mesh;
	//stateの初期値の０が全てのアニメーションネームスペースでは待機という意味を持つので、初期状態が必ず０です。
	int m_state = 0;
	//アニメーションの更新がprevious stateとstateの値が異なった場合のみ更新するので、最初の待機アニメーションを始めるのに‐1に設定します。
	int m_previous_state = -1;

	////アニメーション関連
	//int m_try_animation = -1;
	//int m_queued_animation = -1;

	//ディファドレンダリングに関する変数
	bool m_has_shadow;

protected:
	friend class ActorManager;
};
