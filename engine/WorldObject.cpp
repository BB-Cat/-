#include "WorldObject.h"
#include "VertexMesh.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include "ConstantBufferSystem.h"
#include "IndexBuffer.h"
#include "PrimitiveGenerator.h"
#include "SkinnedMesh.h"
#include "Primitive.h"

WorldObject::WorldObject(PrimitivePtr primitive, int collider_type,
	Vector3D pos, Vector3D scale, Vector3D rot, int shader)
{
	m_primitive = primitive;
	m_object_type = ObjectType::Primitive;

	m_shader = shader;

	m_scale = scale;
	m_pos = pos;
	m_rot = rot;

	//determine if there is a collider and create it if necessary
	switch (collider_type)
	{
	case ColliderTypes::None: 
		break;
	case ColliderTypes::Cube:
		m_collider = new CubeCollider(m_scale);
		break;
	case ColliderTypes::Sphere:
		m_collider = new SphereCollider(m_scale.m_x / 2.0f);
		break;
	case ColliderTypes::Capsule:
		//still needs to be implemented
		break;
	default: 
		break;
	}
}

WorldObject::WorldObject(SkinnedMeshPtr mesh, Collider* collider, Vector3D pos, Vector3D scale, Vector3D rot, int shader)
{
	m_mesh = mesh;
	m_object_type = ObjectType::Mesh;

	m_shader = shader;

	m_scale = scale;
	m_pos = pos;
	m_rot = rot;

	m_collider = collider;
}

WorldObject::~WorldObject()
{
	if (m_collider != nullptr) delete m_collider;
}

void WorldObject::render(float elapsed_time, int shader, bool is_textured)
{
	render(elapsed_time, m_scale, m_pos, m_rot, shader, is_textured);
}

void WorldObject::render(float elapsed_time, Vector3D scale, Vector3D position, Vector3D rotation, int shader, bool is_textured)
{
	int active_shader = m_shader;
	if (shader >= 0) active_shader = shader;

	//The world object will render based on what type of object it is.
	switch (m_object_type)
	{
	case ObjectType::Mesh:
		m_mesh->renderMesh(elapsed_time, scale, position, rotation, active_shader, is_textured);
		break;
	case ObjectType::Primitive:
		m_primitive->render(scale, position, rotation, active_shader, is_textured);
		break;
	}
}

void WorldObject::setMaterial(Material_Obj mat)
{
	switch (m_object_type)
	{
	case ObjectType::Mesh:
		m_mesh->setMaterial(mat);
		break;
	case ObjectType::Primitive:
		m_primitive->setMaterial(mat);
		break;
	}
}

Material_Obj WorldObject::getMaterial()
{
	Material_Obj mat = {};
	switch (m_object_type)
	{
	case ObjectType::Mesh:
		mat = m_mesh->getMaterial();
		break;
	case ObjectType::Primitive:
		mat = m_primitive->getMaterial();
		break;
	}

	return mat;
}

Matrix4x4 WorldObject::applyTransformations(const Matrix4x4& global, Vector3D scale, Vector3D rot, Vector3D translate)
{
	Matrix4x4 out = global;
	Matrix4x4 temp;
	temp.setIdentity();

	//Scale
	temp.setScale(scale);
	out *= temp;

	//Rotation
	temp.setIdentity();
	temp.setRotationZ(rot.m_z);
	out *= temp;
	temp.setIdentity();
	temp.setRotationX(rot.m_x);
	out *= temp;
	temp.setIdentity();
	temp.setRotationY(rot.m_y);
	out *= temp;

	//Translation
	out.setTranslation(translate);

	return out;

}