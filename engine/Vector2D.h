#pragma once
#include <d3d11.h>
#include <cmath> 

class Vec2
{
public:
	Vec2() :x(0), y(0)
	{
	}
	Vec2(float x, float y) :x(x), y(y)
	{
	}
	Vec2(const Vec2& vector) :x(vector.x), y(vector.y)
	{
	}

	static float dot(Vec2 a, Vec2 b)
	{
		return (a.x * b.x + a.y * b.y);
	}

	float length()
	{
		return sqrt(x * x + y * y);
	}

	void normalize()
	{
		float l = length();
		x /= l;
		y /= l;
	}

	Vec2 operator *(float num)
	{
		return Vec2(x * num, y * num);
	}

	Vec2 operator *(Vec2 v2)
	{
		return Vec2(x * v2.x, y * v2.y);
	}

	Vec2 operator +(Vec2 vec)
	{
		return Vec2(x + vec.x, y + vec.y);
	}

	Vec2 operator -(Vec2 vec)
	{
		return Vec2(x - vec.x, y - vec.y);
	}

	Vec2 operator -(float num)
	{
		return Vec2(x - num, y - num);
	}

	Vec2 operator /(float a)
	{
		return Vec2(x / a, y / a);
	}

	bool operator ==(Vec2 vec)
	{
		return (this->x == vec.x && this->y == vec.y);
	}

	bool operator !=(Vec2 vec)
	{
		return (this->x != vec.x || this->y != vec.y);
	}

	~Vec2()
	{
	}

public:
	float x, y;
};