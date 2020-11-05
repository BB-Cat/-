#pragma once
#include <d3d11.h>
#include <cmath> 

class Vector2D
{
public:
	Vector2D() :m_x(0), m_y(0)
	{
	}
	Vector2D(float x, float y) :m_x(x), m_y(y)
	{
	}
	Vector2D(const Vector2D& vector) :m_x(vector.m_x), m_y(vector.m_y)
	{
	}

	static float dot(Vector2D a, Vector2D b)
	{
		return (a.m_x * b.m_x + a.m_y * b.m_y);
	}

	float length()
	{
		return sqrt(m_x * m_x + m_y * m_y);
	}

	void normalize()
	{
		float l = length();
		m_x /= l;
		m_y /= l;
	}

	Vector2D operator *(float num)
	{
		return Vector2D(m_x * num, m_y * num);
	}

	Vector2D operator *(Vector2D v2)
	{
		return Vector2D(m_x * v2.m_x, m_y * v2.m_y);
	}

	Vector2D operator +(Vector2D vec)
	{
		return Vector2D(m_x + vec.m_x, m_y + vec.m_y);
	}

	Vector2D operator -(Vector2D vec)
	{
		return Vector2D(m_x - vec.m_x, m_y - vec.m_y);
	}

	Vector2D operator -(float num)
	{
		return Vector2D(m_x - num, m_y - num);
	}

	Vector2D operator /(float a)
	{
		return Vector2D(m_x / a, m_y / a);
	}

	bool operator !=(Vector2D vec)
	{
		return (this->m_x != vec.m_x || this->m_y != vec.m_y);
	}

	~Vector2D()
	{
	}

public:
	float m_x, m_y;
};