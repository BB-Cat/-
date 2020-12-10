#pragma once
#include "Vector3D.h"
#include "Vector2D.h"
#include "Vector4D.h"

#define MAX_BONE_INFLUENCES (4)

class VertexMesh
{
public:
	VertexMesh() :m_position(), m_texcoord()//, m_bone_influence(), m_bone_indices()
	{
	}
	VertexMesh(Vec3 position, Vec2 texcoord) :m_position(position), m_texcoord(texcoord)
	{
	}
	VertexMesh(Vec3 position, Vec2 texcoord, Vec3 normal) :m_position(position), m_texcoord(texcoord), m_normal(normal)
	{
	}
	VertexMesh(Vec3 position, Vec3 normal, Vector4D influence) :m_position(position), m_texcoord(Vec2(0,0)), m_normal(normal)
	{
		m_bone_influence[0] = influence.m_x;
		m_bone_influence[1] = influence.m_y;
		m_bone_influence[2] = influence.m_z;
		m_bone_influence[3] = influence.m_w;
	}


	VertexMesh(const VertexMesh& vertex) :m_position(vertex.m_position), m_texcoord(vertex.m_texcoord), m_normal(vertex.m_normal)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCES; i++)
		{
			m_bone_influence[i] = vertex.m_bone_influence[i];
			m_bone_indices[i] = vertex.m_bone_indices[i];
		}
	}

	~VertexMesh()
	{
	}

public:
	Vec3 m_position;
	Vec2 m_texcoord;
	Vec3 m_normal;
	float m_bone_influence[MAX_BONE_INFLUENCES] = {};
	int m_bone_indices[MAX_BONE_INFLUENCES] = {};
};