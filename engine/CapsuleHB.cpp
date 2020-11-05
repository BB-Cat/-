#include "CapsuleHB.h"
#include "SkinnedMesh.h"
#include "GraphicsEngine.h"
#include "Matrix4X4.h"
#include "AppWindow.h"

Capsule::Capsule(Vector3D pos, float scale) : m_pos(pos), m_scale(Vector3D(1,1,1)*scale), m_cylinder_len(1.0f * scale), m_radius(1.0f * scale)
{
	m_mesh = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\OBJ\\Okk.fbx", true, nullptr);
}

Capsule::~Capsule()
{
}

void Capsule::update(high_resolution_timer delta,  Terrain* terrain, Vector3D cap1, Vector3D cap2, float radius)
{
	m_color = Vector3D(0, 0, 1);


	if (terrain != nullptr) //just using the unnecessary terrain value as an easy way to seperate the capsules for testing
	{

			//horizontal inertia calculation
		if (AppWindow::getKeyState('I')) m_pos.m_z += 0.05f;
		else if (AppWindow::getKeyState('K')) m_pos.m_z -= 0.05f;

		if (AppWindow::getKeyState('J'))  m_pos.m_x += 0.05f;
		else if (AppWindow::getKeyState('L'))  m_pos.m_x -= 0.05f;

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
		m_color = Vector3D(1, 0, 0);


}

void Capsule::render(high_resolution_timer delta)
{
//	m_mesh->renderMesh(delta.time_interval(), m_scale, m_pos, Vector3D(0, m_angleY * 0.01745f, m_angleX * 0.01745f), m_color);

}

bool Capsule::checkCollisionCapsule(Vector3D cap1, Vector3D cap2, float radius)
{
	float dist = calculateSegmentSegmentDistance(getCapPos(true), getCapPos(false), cap1, cap2);

	if (m_radius + radius > dist) return true;
	else return false;
}

float Capsule::calculateLineLineDistance(Vector3D p1, Vector3D e1, Vector3D p2, Vector3D e2, float& dist_from_l1, float& dist_from_l2)
{
	

	//calculate to see if parallel or not
	Vector3D v1 = e1 - p1;
	v1.normalize();
	Vector3D v2 = e2 - p2;
	v2.normalize();

	float dot1 = Vector3D::dot(p2 - p1, v1);
	float dot2 = Vector3D::dot(p2 - p1, v2);

	Vector3D cross = Vector3D::cross(v1, v2);
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

	float DV1V2 = Vector3D::dot(v1, v2); //l1.v.dot(l2.v);
	float DV1V1 = v1.length() * v1.length();
	float DV2V2 = v2.length() * v2.length();
	Vector3D P21P11 = p1 - p2;
	dist_from_l1 = ( DV1V2 * Vector3D::dot(v2, P21P11) - DV2V2 * Vector3D::dot(v1, P21P11) ) / (DV1V1 * DV2V2 - DV1V2 * DV1V2);
	Vector3D finalp1 = p1 + v1 * dist_from_l1;
	dist_from_l2 = Vector3D::dot(v2, finalp1 - p2) / DV2V2;
	Vector3D finalp2 = p2 + v2 * dist_from_l2;

	return (finalp2 - finalp1).length();
}

float Capsule::calculateSegmentSegmentDistance(Vector3D p1, Vector3D e1, Vector3D p2, Vector3D e2)
{
	//find the shortest distance between the two lines which make the core of each capsule
	//Vector3D p1 = getCapPos(false);
	//Vector3D p2 = cap1;

	float closest1, closest2;

	//calculate to see if parallel or not
	Vector3D v1 = e1 - p1;
	v1.normalize();
	Vector3D v2 = e2 - p2;
	v2.normalize();

	float dot1 = Vector3D::dot(p2 - p1, v1);
	float dot2 = Vector3D::dot(p2 - p1, v2);

	Vector3D cross = Vector3D::cross(v1, v2);
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
	Vector3D p1point = p1 + v1 * closest1;
	float len = calculatePointSegmentDistance(p1point, p2, e2, closest2);
	if (0.0f <= closest2 && closest2 <= 1.0f)
		return len;


	closest2 = max(min(closest2, 1.0f), 0.0f);
	v2 = e2 - p2;
	Vector3D p2point = p2 + v2 * closest2;
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

float Capsule::calculatePointPointDistance(Vector3D point1, Vector3D point2)
{
	Vector3D dist = point2 - point1;
	return dist.length();
}

float Capsule::calculatePointLineDistance(Vector3D point1, Vector3D line_p1, Vector3D line_p2, float& dist_from_l1)
{
		Vector3D line_vec = line_p2 - line_p1;
		float line_len = line_vec.length();
		float line_len_sq = line_len * line_len;
		dist_from_l1 = 0.0f;
		if(line_len_sq > 0.0f) dist_from_l1 = Vector3D::dot((point1 - line_p1), line_vec) / line_len_sq;

		Vector3D line_closest = line_p1 + line_vec * dist_from_l1;

		return (point1 - line_closest).length();
}

float Capsule::calculatePointSegmentDistance(Vector3D point1, Vector3D line_p1, Vector3D line_p2, float& dist_from_l1)
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

bool Capsule::isAcute(const Vector3D& v1, const Vector3D& v2)
{
	return (Vector3D::dot(v1, v2) >= 0.0f);
}

Vector3D Capsule::getCapPos(bool bottom)
{
	if (bottom) return m_cap2 + m_pos;
	else return m_cap1 + m_pos;
}

Vector3D Capsule::calculateCapPos(bool bottom) //the bool is to calculate if it is the top cap or bottom cap
{
	Vector3D pos = Vector3D(0.0f, m_cylinder_len * (1 - 2 * bottom), 0.0f);

	Matrix4x4 rot;
	rot.setIdentity();
	rot.setRotationZ(m_angleX * 0.01745f);
	pos = rot * pos;

	rot.setIdentity();
	rot.setRotationY(m_angleY * 0.01745f);
	pos = rot * pos;

	return pos;
}
