#pragma once
#include "Vector3D.h"

class Vector4D
{
public:
	Vector4D() : m_x(0), m_y(0), m_z(0),m_w(0)
	{
	}
	Vector4D(float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w)
	{
	}
	Vector4D(const Vector4D& vector4d) : m_x(vector4d.m_x), m_y(vector4d.m_y), m_z(vector4d.m_z), m_w(vector4d.m_w)
	{
	}

	Vector4D(const Vector3D& vector3d) : m_x(vector3d.m_x), m_y(vector3d.m_y), m_z(vector3d.m_z), m_w(1.0f)
	{
	}

	void cross(Vector4D& v1, Vector4D& v2, Vector4D& v3)
	{
		this->m_x = v1.m_y * (v2.m_z * v3.m_w - v3.m_z * v2.m_w) - v1.m_z * (v2.m_y * v3.m_w - v3.m_y * v2.m_w) + v1.m_w * (v2.m_y * v3.m_z - v2.m_z * v3.m_y);
		this->m_y = -(v1.m_x * (v2.m_z * v3.m_w - v3.m_z * v2.m_w) - v1.m_z * (v2.m_x * v3.m_w - v3.m_x * v2.m_w) + v1.m_w * (v2.m_x * v3.m_z - v3.m_x * v2.m_z));
		this->m_z = v1.m_x * (v2.m_y * v3.m_w - v3.m_y * v2.m_w) - v1.m_y * (v2.m_x * v3.m_w - v3.m_x * v2.m_w) + v1.m_w * (v2.m_x * v3.m_y - v3.m_x * v2.m_y);
		this->m_w = -(v1.m_x * (v2.m_y * v3.m_z - v3.m_y * v2.m_z) - v1.m_y * (v2.m_x * v3.m_z - v3.m_x * v2.m_z) + v1.m_z * (v2.m_x * v3.m_y - v3.m_x * v2.m_y));
	}

	Vector3D xyz()
	{
		return Vector3D(m_x, m_y, m_z);
	}

	~Vector4D()
	{
	}

	//void operator = (const float *num)
	//{
	//	m_x = num[0];
	//	m_y = num[1];
	//	m_z = num[2];
	//	m_w = num[3];
	//}

	void loadFloat(const float* num)
	{
			m_x = num[0];
			m_y = num[1];
			m_z = num[2];
			m_w = num[3];
	}

	Vector4D operator / (float num)
	{
		return(Vector4D(m_x / num, m_y / num, m_z / num, m_w / num));
	}

	Vector4D operator * (float num)
	{
		return(Vector4D(m_x * num, m_y * num, m_z * num, m_w * num));
	}

	Vector4D operator * (Vector4D vec)
	{
		return(Vector4D(m_x * vec.m_x, m_y * vec.m_y, m_z * vec.m_z, m_w * vec.m_w));
	}

	Vector4D operator + (float num)
	{
		return(Vector4D(m_x + num, m_y + num, m_z + num, m_w + num));
	}

	Vector4D operator + (Vector4D v2)
	{
		return(Vector4D(m_x + v2.m_x, m_y + v2.m_y, m_z + v2.m_z, m_w + v2.m_w));
	}


public:
	float m_x, m_y, m_z, m_w;

};
