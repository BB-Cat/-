#pragma once
#include <d3d11.h>
#include "Vector2D.h"

class Vector3D
{
public:
	Vector3D() : m_x(0), m_y(0), m_z(0)
	{
	}
	Vector3D(float x, float y, float z) : m_x(x), m_y(y), m_z(z)
	{
	}
	Vector3D(const Vector3D& vector3d) : m_x(vector3d.m_x), m_y(vector3d.m_y), m_z(vector3d.m_z)
	{
	}
	Vector3D(const Vector2D& vector2d) : m_x(vector2d.m_x), m_y(vector2d.m_y), m_z(0.0f)
	{
	}

	static float dot(Vector3D a, Vector3D b)
	{
		return (a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z);
	}

	static Vector3D cross(Vector3D& v1, Vector3D& v2)
	{
		Vector3D temp;
		temp.m_x = v1.m_y * v2.m_z - v1.m_z * v2.m_y;;
		temp.m_y = -(v1.m_x * v2.m_z - v1.m_z * v2.m_x);;
		temp.m_z = v1.m_x * v2.m_y - v1.m_y * v2.m_x;;

		return temp;
	}

	static Vector3D lerp(const Vector3D& start, const Vector3D& end, float delta)
	{
		Vector3D v;
		v.m_x = start.m_x * (1.0f - delta) + end.m_x * (delta);
		v.m_y = start.m_y * (1.0f - delta) + end.m_y * (delta);
		v.m_z = start.m_z * (1.0f - delta) + end.m_z * (delta);

		return v;
	}

	static Vector3D slerp(Vector3D start, Vector3D end, float delta)
	{
		Vector3D relative;
		float dot = Vector3D::dot(start, end);
		dot = min(max(dot, -1), 1);

		float theta = acosf(dot) * delta;
		relative = end - (start * theta);
		relative.normalize();

		return (start*cosf(theta)) + (relative*sinf(theta));
	}

	float length()
	{
		return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
	}

	void normalize()
	{
		float l = sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
		m_x /= l;
		m_y /= l;
		m_z /= l;
	}

	//float operators
	Vector3D operator +(float num)
	{
		return(Vector3D(m_x + num, m_y + num, m_z + num));
	}
	Vector3D operator -(float num)
	{
		return(Vector3D(m_x - num, m_y - num, m_z - num));
	}

	Vector3D operator *(float num)
	{
		return(Vector3D(m_x * num, m_y * num, m_z * num));
	}
	Vector3D operator /(float num)
	{
		return(Vector3D(m_x / num, m_y / num, m_z / num));
	}

	void operator +=(const float& num)
	{
		m_x = m_x + num;
		m_y = m_y + num;
		m_z = m_z + num;
	}

	void operator -=(const Vector3D& v3)
	{
		m_x = m_x - v3.m_x;
		m_y = m_y - v3.m_y;
		m_z = m_z - v3.m_z;
	}

	bool operator ==(const Vector3D& num)
	{
		if (m_x != num.m_x) return false;
		if (m_y != num.m_y) return false;
		if (m_z != num.m_z) return false;
		
		return true;
	}



	//vector3D operators
	Vector3D operator +(Vector3D v2)
	{
		return(Vector3D(m_x + v2.m_x, m_y + v2.m_y, m_z + v2.m_z));
	}
	Vector3D operator -(Vector3D v2)
	{
		return(Vector3D(m_x - v2.m_x, m_y - v2.m_y, m_z - v2.m_z));
	}

	Vector3D operator *(Vector3D v2)
	{
		return(Vector3D(m_x * v2.m_x, m_y * v2.m_y, m_z * v2.m_z));
	}
	Vector3D operator /(Vector3D v2)
	{
		return(Vector3D(m_x / v2.m_x, m_y / v2.m_y, m_z / v2.m_z));
	}

	void operator +=(const Vector3D& v3)
	{
		m_x = m_x + v3.m_x;
		m_y = m_y + v3.m_y;
		m_z = m_z + v3.m_z;
	}

	~Vector3D()
	{
	}



public:
	float m_x, m_y, m_z;

};
