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
	VertexMesh(Vector3D position, Vector2D texcoord) :m_position(position), m_texcoord(texcoord)
	{
	}
	VertexMesh(Vector3D position, Vector2D texcoord, Vector3D normal) :m_position(position), m_texcoord(texcoord), m_normal(normal)
	{
	}
	VertexMesh(Vector3D position, Vector3D normal, Vector4D influence) :m_position(position), m_texcoord(Vector2D(0,0)), m_normal(normal)
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
	Vector3D m_position;
	Vector2D m_texcoord;
	Vector3D m_normal;
	float m_bone_influence[MAX_BONE_INFLUENCES] = {};
	int m_bone_indices[MAX_BONE_INFLUENCES] = {};
};