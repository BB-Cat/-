#pragma once
#include <d3d11.h>
#include "Vector2D.h"

class Vec3
{
public:
	Vec3() : x(0), y(0), z(0) {}

	Vec3(float val) : x(val), y(val), z(val) {}

	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vec3(const Vec3& vector3d) : x(vector3d.x), y(vector3d.y), z(vector3d.z) {}

	Vec3(const Vec2& vector2d) : x(vector2d.x), y(vector2d.y), z(0.0f) {}

	static float dot(Vec3 a, Vec3 b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	static Vec3 cross(Vec3& v1, Vec3& v2)
	{
		Vec3 temp;
		temp.x = v1.y * v2.z - v1.z * v2.y;;
		temp.y = -(v1.x * v2.z - v1.z * v2.x);;
		temp.z = v1.x * v2.y - v1.y * v2.x;;

		return temp;
	}

	static Vec3 lerp(const Vec3& start, const Vec3& end, float delta)
	{
		Vec3 v;
		v.x = start.x * (1.0f - delta) + end.x * (delta);
		v.y = start.y * (1.0f - delta) + end.y * (delta);
		v.z = start.z * (1.0f - delta) + end.z * (delta);

		return v;
	}

	static Vec3 slerp(Vec3 start, Vec3 end, float delta)
	{
		Vec3 relative;
		float dot = Vec3::dot(start, end);
		dot = min(max(dot, -1), 1);

		float theta = acosf(dot) * delta;
		relative = end - (start * theta);
		relative.normalize();

		return (start*cosf(theta)) + (relative*sinf(theta));
	}

	float length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	void normalize()
	{
		float l = sqrt(x * x + y * y + z * z);
		x /= l;
		y /= l;
		z /= l;
	}

	//float operators
	Vec3 operator +(float num)
	{
		return(Vec3(x + num, y + num, z + num));
	}
	Vec3 operator -(float num)
	{
		return(Vec3(x - num, y - num, z - num));
	}

	Vec3 operator *(float num)
	{
		return(Vec3(x * num, y * num, z * num));
	}
	Vec3 operator /(float num)
	{
		return(Vec3(x / num, y / num, z / num));
	}

	void operator +=(const float& num)
	{
		x = x + num;
		y = y + num;
		z = z + num;
	}

	void operator -=(const Vec3& v3)
	{
		x = x - v3.x;
		y = y - v3.y;
		z = z - v3.z;
	}

	bool operator ==(const Vec3& num)
	{
		if (x != num.x) return false;
		if (y != num.y) return false;
		if (z != num.z) return false;
		
		return true;
	}



	//vector3D operators
	Vec3 operator +(Vec3 v2)
	{
		return(Vec3(x + v2.x, y + v2.y, z + v2.z));
	}
	Vec3 operator -(Vec3 v2)
	{
		return(Vec3(x - v2.x, y - v2.y, z - v2.z));
	}

	Vec3 operator *(Vec3 v2)
	{
		return(Vec3(x * v2.x, y * v2.y, z * v2.z));
	}
	Vec3 operator /(Vec3 v2)
	{
		return(Vec3(x / v2.x, y / v2.y, z / v2.z));
	}

	void operator +=(const Vec3& v3)
	{
		x = x + v3.x;
		y = y + v3.y;
		z = z + v3.z;
	}

	~Vec3()
	{
	}



public:
	float x, y, z;

};
