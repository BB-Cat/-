#include "CapsuleHB.h"
#include "SkinnedMesh.h"
#include "GraphicsEngine.h"
#include "Matrix4X4.h"
#include "AppWindow.h"

Capsule::Capsule(Vec3 pos, float scale) : m_pos(pos), m_scale(Vec3(1,1,1)*scale), m_cylinder_len(1.0f * scale), m_radius(1.0f * scale)
{
	m_mesh = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\OBJ\\Okk.fbx", true, nullptr);
}

Capsule::~Capsule()
{
}

void Capsule::update(high_resolution_timer delta,  Terrain* terrain, Vec3 cap1, Vec3 cap2, float radius)
{
	m_color = Vec3(0, 0, 1);


	if (terrain != nullptr) //just using the unnecessary terrain value as an easy way to seperate the capsules for testing
	{

			//horizontal inertia calculation
		if (AppWindow::getKeyState('I')) m_pos.z += 0.05f;
		else if (AppWindow::getKeyState('K')) m_pos.z -= 0.05f;

		if (AppWindow::getKeyState('J'))  m_pos.x += 0.05f;
		else if (AppWindow::getKeyState('L'))  m_pos.x -= 0.05f;

		if (AppWindow::getKeyState('7')) m_angleX += 2.0f;
		else if (AppWindow::getKeyState('8')) m_angleX -= 2.0f;

		if (AppWindow::getKeyState('9')) m_angleY += 2.0f;
		else if (AppWindow::getKeyState('0')) m_angleY -= 2.0f;
	}


	

	//update the stored positions of the caps
	m_cap1 = calculateCapPos(false);
	m_cap2 = calculateCapPos(true);

	if (
		checkCollisionCapsule(cap1, cap2, radius)
		) 
		m_color = Vec3(1, 0, 0);


}

void Capsule::render(high_resolution_timer delta)
{
//	m_mesh->renderMesh(delta.time_interval(), m_scale, m_pos, Vector3D(0, m_angleY * 0.01745f, m_angleX * 0.01745f), m_color);

}

bool Capsule::checkCollisionCapsule(Vec3 cap1, Vec3 cap2, float radius)
{
	float dist = calculateSegmentSegmentDistance(getCapPos(true), getCapPos(false), cap1, cap2);

	if (m_radius + radius > dist) return true;
	else return false;
}

float Capsule::calculateLineLineDistance(Vec3 p1, Vec3 e1, Vec3 p2, Vec3 e2, float& dist_from_l1, float& dist_from_l2)
{
	

	//calculate to see if parallel or not
	Vec3 v1 = e1 - p1;
	v1.normalize();
	Vec3 v2 = e2 - p2;
	v2.normalize();

	float dot1 = Vec3::dot(p2 - p1, v1);
	float dot2 = Vec3::dot(p2 - p1, v2);

	Vec3 cross = Vec3::cross(v1, v2);
	float dV = cross.length();

	//if the lines are parallel, return their distance
	if (dV < 0.000001f)
	{
		float len = calculatePointLineDistance(p1, p2, e2, dist_from_l2);
		dist_from_l1 = 0.0f;

		return len;
	}


	v1 = e1 - p1;
	v2 = e2 - p2;

	float DV1V2 = Vec3::dot(v1, v2); //l1.v.dot(l2.v);
	float DV1V1 = v1.length() * v1.length();
	float DV2V2 = v2.length() * v2.length();
	Vec3 P21P11 = p1 - p2;
	dist_from_l1 = ( DV1V2 * Vec3::dot(v2, P21P11) - DV2V2 * Vec3::dot(v1, P21P11) ) / (DV1V1 * DV2V2 - DV1V2 * DV1V2);
	Vec3 finalp1 = p1 + v1 * dist_from_l1;
	dist_from_l2 = Vec3::dot(v2, finalp1 - p2) / DV2V2;
	Vec3 finalp2 = p2 + v2 * dist_from_l2;

	return (finalp2 - finalp1).length();
}

float Capsule::calculateSegmentSegmentDistance(Vec3 p1, Vec3 e1, Vec3 p2, Vec3 e2)
{
	//find the shortest distance between the two lines which make the core of each capsule
	//Vector3D p1 = getCapPos(false);
	//Vector3D p2 = cap1;

	float closest1, closest2;

	//calculate to see if parallel or not
	Vec3 v1 = e1 - p1;
	v1.normalize();
	Vec3 v2 = e2 - p2;
	v2.normalize();

	float dot1 = Vec3::dot(p2 - p1, v1);
	float dot2 = Vec3::dot(p2 - p1, v2);

	Vec3 cross = Vec3::cross(v1, v2);
	float dV = cross.length();

	//if the lines are parallel, return their distance
	if (dV < 0.000001f)
	{
		////D3DXVECTOR3 v;
		//Vector3D v = Vector3D::cross(p2 - p1, v1);
		//return v.length();
		closest1 = 0.0f;
		float len = calculatePointSegmentDistance(p1, p2, e2, closest2);
		if (0.0f <= closest2 && closest2 <= 1.0f)
			return len;
	}
	else {
		//float len = calcLineLineDist(s1, s2, p1, p2, t1, t2);
		float len = calculateLineLineDistance(p1, e1, p2, e2, closest1, closest2);
		if (
			0.0f <= closest1 && closest1 <= 1.0f &&
			0.0f <= closest2 && closest2 <= 1.0f
			) 
		{
			return len;
		}
	}

	closest1 = max(min(closest1, 1.0f), 0.0f);
	v1 = e1 - p1;
	Vec3 p1point = p1 + v1 * closest1;
	float len = calculatePointSegmentDistance(p1point, p2, e2, closest2);
	if (0.0f <= closest2 && closest2 <= 1.0f)
		return len;


	closest2 = max(min(closest2, 1.0f), 0.0f);
	v2 = e2 - p2;
	Vec3 p2point = p2 + v2 * closest2;
	len = calculatePointSegmentDistance(p2point, p1, e1, closest1);
	if (0.0f <= closest1 && closest1 <= 1.0f)
		return len;


	closest1 = max(min(closest1, 1.0f), 0.0f);
	p1point = p1 + v1 * closest1;
	p2point = p2 + v2 * closest2;

	return (p2point - p1point).length();

	//Vector3D vec1 = getCapPos(true) - getCapPos(false);
	//Vector3D vec2 = cap2 - cap1;

	//vec1.normalize();
	//vec2.normalize();

	//float dot1 = Vector3D::dot(p2 - p1, vec1);
	//float dot2 = Vector3D::dot(p2 - p1, vec2);

	//Vector3D cross = Vector3D::cross(vec1, vec2);
	//float dV = cross.length();

	////if the lines are parallel, return their distance
	//if (dV < 0.000001f)
	//{
	//	//D3DXVECTOR3 v;
	//	Vector3D v = Vector3D::cross(p2 - p1, vec1);
	//	return v.length();
	//}

	//dV = Vector3D::dot(vec1, vec2);
	//float t1 = (dot1 - dot2 * dV) / (1.0f - dV * dV);
	//float t2 = (dot2 - dot1 * dV) / (dV * dV - 1.0f);

	////t1 = max(min(t1, 1.0f), 0.0f);
	////t2 = max(min(t2, 1.0f), 0.0f);

	////calculate the closest relative positions on each line
	//Vector3D Q1 = p1 + vec1 * t1;
	//Vector3D Q2 = p2 + vec2 * t2;

	//if (t1 >= 0.0f && t1 <= 1.0f && t2 >= 0.0f && t2 <= 1.0f)
	//{
	//	Vector3D final_vec = Q2 - Q1;
	//	return final_vec.length();
	//}

	//t1 = max(min(t1, 1.0f), 0.0f);
	//Q1 = p1 + vec1 * t1;
	//float len = //calcPointSegmentDist(p1, s2, p2, t2);
	//if (0.0f <= t2 && t2 <= 1.0f)
	//	return len;

	////Vector3D final_vec = Q2 - Q1;
	////return final_vec.length();
}

float Capsule::calculatePointPointDistance(Vec3 point1, Vec3 point2)
{
	Vec3 dist = point2 - point1;
	return dist.length();
}

float Capsule::calculatePointLineDistance(Vec3 point1, Vec3 line_p1, Vec3 line_p2, float& dist_from_l1)
{
		Vec3 line_vec = line_p2 - line_p1;
		float line_len = line_vec.length();
		float line_len_sq = line_len * line_len;
		dist_from_l1 = 0.0f;
		if(line_len_sq > 0.0f) dist_from_l1 = Vec3::dot((point1 - line_p1), line_vec) / line_len_sq;

		Vec3 line_closest = line_p1 + line_vec * dist_from_l1;

		return (point1 - line_closest).length();
}

float Capsule::calculatePointSegmentDistance(Vec3 point1, Vec3 line_p1, Vec3 line_p2, float& dist_from_l1)
{
	float len = calculatePointLineDistance(point1, line_p1, line_p2, dist_from_l1);

	if (isAcute((point1 - line_p1), (line_p2 - line_p1)) == false)
	{
		return (line_p1 - point1).length();
	}
	else if(isAcute((point1 - line_p2), (line_p1 - line_p2)) == false)
	{
		return (line_p2 - point1).length();
	}

	return len;
}

bool Capsule::isAcute(const Vec3& v1, const Vec3& v2)
{
	return (Vec3::dot(v1, v2) >= 0.0f);
}

Vec3 Capsule::getCapPos(bool bottom)
{
	if (bottom) return m_cap2 + m_pos;
	else return m_cap1 + m_pos;
}

Vec3 Capsule::calculateCapPos(bool bottom) //the bool is to calculate if it is the top cap or bottom cap
{
	Vec3 pos = Vec3(0.0f, m_cylinder_len * (1 - 2 * bottom), 0.0f);

	Matrix4x4 rot;
	rot.setIdentity();
	rot.setRotationZ(m_angleX * 0.01745f);
	pos = rot * pos;

	rot.setIdentity();
	rot.setRotationY(m_angleY * 0.01745f);
	pos = rot * pos;

	return pos;
}
