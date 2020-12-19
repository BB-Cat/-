#include "Colliders.h"
#include "Matrix4X4.h"

Vec3 Collider::getBoundingBox()
{
	CubeCollider* c;
	SphereCollider* s;

	switch (m_type)
	{
	case ColliderTypes::Cube:
		c = reinterpret_cast<CubeCollider*>(this);
		return c->getBoundingBox();

	case ColliderTypes::Sphere:
		s = reinterpret_cast<SphereCollider*>(this);
		return s->getBoundingBox();

	case ColliderTypes::Capsule:
		break;

	case ColliderTypes::None:
		//in the case of no collider, return a 0,0,0 value.
		return Vec3();

	default:
		OutputDebugString(L"Collider type not recognized in getBoundingBox function.\n");
	}

	return Vec3(-1, -1, -1);
}

void Collider::setBoundingBox(Vec3 box)
{
	CubeCollider* c;
	SphereCollider* s;

	switch (m_type)
	{
	case ColliderTypes::Cube:
		c = reinterpret_cast<CubeCollider*>(this);
		c->setBoundingBox(box);
		break;

	case ColliderTypes::Sphere:
		s = reinterpret_cast<SphereCollider*>(this);
		return s->setBoundingBox(box);

	case ColliderTypes::Capsule:
		break;

	case ColliderTypes::None:
		OutputDebugString(L"Attempted to change collider size of an empty collider!!\n");

	default:
		OutputDebugString(L"Collider type not recognized in setBoundingBox function.\n");
	}
}

void SphereCollider::setBoundingBox(Vec3 box)
{
#ifdef DEBUG
	if (box.x != box.y || box.x != box.z)
	{
		OutputDebugString(L"POSSIBLE ERROR: Sphere collider is being resized with a disproportionate cube!\n");
	}
#endif

	m_radius = box.x;
}

Vec3 CapsuleCollider::getBoundingBox()
{
	//make sure the caps are updated properly
	if (m_was_updated)
	{
		m_was_updated = false;
		calcCapPositions();
	}

	Vec3 box;
	float high_x, low_x, high_y, low_y, high_z, low_z;
	if (m_cap1.x >= m_cap2.x)
	{
		high_x = m_cap1.x;
		low_x = m_cap2.x;
	}
	else
	{
		high_x = m_cap2.x;
		low_x = m_cap1.x;
	}

	if (m_cap1.y >= m_cap2.y)
	{
		high_y = m_cap1.y;
		low_y = m_cap2.y;
	}
	else
	{
		high_y = m_cap2.y;
		low_y = m_cap1.y;
	}

	if (m_cap1.z >= m_cap2.z)
	{
		high_z = m_cap1.z;
		low_z = m_cap2.z;
	}
	else
	{
		high_z = m_cap2.z;
		low_z = m_cap1.z;
	}

	box.x = high_x - low_x + m_radius * 2;
	box.y = high_y - low_y + m_radius * 2;
	box.z = high_z - low_z + m_radius * 2;

	return box;
}

void CapsuleCollider::getCaps(Vec3& cap1, Vec3& cap2)
{
	//make sure the caps are updated properly
	if (m_was_updated)
	{
		m_was_updated = false;
		calcCapPositions();
	}

	cap1 = m_cap1;
	cap2 = m_cap2;
}

void CapsuleCollider::calcCapPositions()
{
	m_cap1 = Vec3(0.0f, m_core_length * 0.5f, 0.0f);
	m_cap2 = Vec3(0.0f, m_core_length * -0.5f, 0.0f);

	//m_cap1 = Vec3(m_core_length * 0.5f, 0, 0.0f);
	//m_cap2 = Vec3(m_core_length * -0.5f, 0, 0.0f);

	//rotation matrix to rotate the caps in local space
	Matrix4x4 transform;
	transform.setIdentity();
	Matrix4x4 rot;
	rot.setIdentity();
	//rot.setRotationZ(m_rotation.z);
	////m_cap1 = rot * m_cap1;
	////m_cap2 = rot * m_cap2;
	//transform *= rot;

	//rot.setIdentity();
	//rot.setRotationX(m_rotation.x);
	////m_cap1 = rot * m_cap1;
	////m_cap2 = rot * m_cap2;
	//transform *= rot;

	//rot.setIdentity();
	//rot.setRotationY(m_rotation.y);
	////m_cap1 = rot * m_cap1;
	////m_cap2 = rot * m_cap2;
	//transform *= rot;

	//TODO: Fix the gimbal lock and coordinate system problem in the capsule collider!
	rot.setRotationZ(-m_rotation.z);
	m_cap1 = rot * m_cap1;
	m_cap2 = rot * m_cap2;

	//TODO: Fix the gimbal lock and coordinate system problem in the capsule collider!
	rot.setIdentity();
	rot.setRotationX(0);
	m_cap1 = rot * m_cap1;
	m_cap2 = rot * m_cap2;

	//TODO: Fix the gimbal lock and coordinate system problem in the capsule collider!
	rot.setIdentity();
	rot.setRotationY(-m_rotation.y);
	transform *= rot;
	m_cap1 = rot * m_cap1;
	m_cap2 = rot * m_cap2;


	//m_cap1 = transform * m_cap1;
	//m_cap2 = transform * m_cap2;
}
