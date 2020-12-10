#include "Colliders.h"

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