#pragma once
#include <memory>
#include "Vector3D.h"
#include "Vector4D.h"
#include <DirectXMath.h>

class Matrix4x4
{
public:
	Matrix4x4() 
	{
	}
	~Matrix4x4()
	{
	}

	void setIdentity()
	{
		::memset(m_mat, 0, sizeof(float) * 16);
		m_mat[0][0] = 1;
		m_mat[1][1] = 1;
		m_mat[2][2] = 1;
		m_mat[3][3] = 1;
	}

	void setTranslation(const Vec3& translation)
	{
		m_mat[3][0] = translation.x;
		m_mat[3][1] = translation.y;
		m_mat[3][2] = translation.z;
	}

	void setScale(const Vec3& scale)
	{
		m_mat[0][0] = scale.x;
		m_mat[1][1] = scale.y;
		m_mat[2][2] = scale.z;
	}

	void setRotationX(float x)
	{
		m_mat[1][1] = cos(x);
		m_mat[1][2] = sin(x);
		m_mat[2][1] = -sin(x);
		m_mat[2][2] = cos(x);
	}

	void setRotationY(float y)
	{
		m_mat[0][0] = cos(y);
		m_mat[0][2] = -sin(y);
		m_mat[2][0] = sin(y);
		m_mat[2][2] = cos(y);
	}

	void setRotationZ(float z)
	{
		m_mat[0][0] = cos(z);
		m_mat[0][1] = sin(z);
		m_mat[1][0] = -sin(z);
		m_mat[1][1] = cos(z);
	}

	float getDeterminant()
	{
		Vector4D minor, v1, v2, v3;
		float det;

		v1 = Vector4D(this->m_mat[0][0], this->m_mat[1][0], this->m_mat[2][0], this->m_mat[3][0]);
		v2 = Vector4D(this->m_mat[0][1], this->m_mat[1][1], this->m_mat[2][1], this->m_mat[3][1]);
		v3 = Vector4D(this->m_mat[0][2], this->m_mat[1][2], this->m_mat[2][2], this->m_mat[3][2]);


		minor.cross(v1, v2, v3);
		det = -(this->m_mat[0][3] * minor.m_x + this->m_mat[1][3] * minor.m_y + this->m_mat[2][3] * minor.m_z +
			this->m_mat[3][3] * minor.m_w);
		return det;
	}

	void inverse()
	{
		int a, i, j;
		Matrix4x4 out;
		Vector4D v, vec[3];
		float det = 0.0f;

		det = this->getDeterminant();
		if (!det) return;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (j != i)
				{
					a = j;
					if (j > i) a = a - 1;
					vec[a].m_x = (this->m_mat[j][0]);
					vec[a].m_y = (this->m_mat[j][1]);
					vec[a].m_z = (this->m_mat[j][2]);
					vec[a].m_w = (this->m_mat[j][3]);
				}
			}
			v.cross(vec[0], vec[1], vec[2]);

			out.m_mat[0][i] = pow(-1.0f, i) * v.m_x / det;
			out.m_mat[1][i] = pow(-1.0f, i) * v.m_y / det;
			out.m_mat[2][i] = pow(-1.0f, i) * v.m_z / det;
			out.m_mat[3][i] = pow(-1.0f, i) * v.m_w / det;
		}

		this->setMatrix(out);
	}

	void lookAt(Vec3 target, Vec3 pos, Vec3 up)
	{
		Vec3 forward = target - pos;
		forward.normalize();
		Vec3 side = Vec3::cross(forward, up);
		side.normalize();
		Vec3 upward = Vec3::cross(forward, side);

		setIdentity();

		m_mat[0][0] = side.x;
		m_mat[0][1] = side.y;
		m_mat[0][2] = side.z;

		m_mat[1][0] = upward.x;
		m_mat[1][1] = upward.y;
		m_mat[1][2] = upward.z;

		m_mat[2][0] = forward.x;
		m_mat[2][1] = forward.y;
		m_mat[2][2] = forward.z;

		m_mat[3][0]	 = 0;
		m_mat[3][1]	 = 0;
		m_mat[3][2]	 = 0;

	}

	float getRoll()
	{
		//roll = arctan2(A31, A32)
		return atan2(m_mat[2][1], m_mat[2][2]);
	}
	
	float getPitch()
	{
		//pitch = arccos(A33)
		//return acos(m_mat[2][2]);
		return -atan2(-m_mat[2][0], sqrt(m_mat[2][1] * m_mat[2][1] + m_mat[2][2] * m_mat[2][2]));
	}

	float getYaw()
	{
		//yaw = -arctan2(A13, A23)
		return atan2(m_mat[1][0], m_mat[0][0]);
	}


	void operator *=(const Matrix4x4& matrix)
	{
		Matrix4x4 output;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				output.m_mat[i][j] =
					m_mat[i][0] * matrix.m_mat[0][j] + m_mat[i][1] * matrix.m_mat[1][j] +
					m_mat[i][2] * matrix.m_mat[2][j] + m_mat[i][3] * matrix.m_mat[3][j];
			}
		}

		setMatrix(output);
	}

	Matrix4x4 operator *(const Matrix4x4& matrix)
	{
		Matrix4x4 output;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				output.m_mat[i][j] =
					m_mat[i][0] * matrix.m_mat[0][j] + m_mat[i][1] * matrix.m_mat[1][j] +
					m_mat[i][2] * matrix.m_mat[2][j] + m_mat[i][3] * matrix.m_mat[3][j];
			}
		}

		return output;
	}

	Matrix4x4 operator +(const Matrix4x4& matrix)
	{
		Matrix4x4 output;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				output.m_mat[i][j] = m_mat[i][j] + matrix.m_mat[i][j];
			}
		}

		return output;
	}

	Matrix4x4 operator *(const float num)
	{
		Matrix4x4 output;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				output.m_mat[i][j] = m_mat[i][j] * num;
			}
		}

		return output;
	}

	void operator =(const DirectX::XMFLOAT4X4& matrix)
	{
		Matrix4x4 output;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				output.m_mat[i][j] = matrix.m[i][j];
			}
		}

		setMatrix(output);
	}

	Vec3 operator *(const Vec3& v3)
	{
		Vec3 output;

		output.x = v3.x * m_mat[0][0] + v3.y * m_mat[0][1] + v3.z * m_mat[0][2] + 1.0f * m_mat[0][3];
		output.y = v3.x * m_mat[1][0] + v3.y * m_mat[1][1] + v3.z * m_mat[1][2] + 1.0f * m_mat[1][3];
		output.z = v3.x * m_mat[2][0] + v3.y * m_mat[2][1] + v3.z * m_mat[2][2] + 1.0f * m_mat[2][3];
		
		return output;
	}

	void operator *=(const DirectX::XMFLOAT4X4& matrix)
	{
		Matrix4x4 output;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				output.m_mat[i][j] =
					m_mat[i][0] * matrix.m[0][j] + m_mat[i][1] * matrix.m[1][j] +
					m_mat[i][2] * matrix.m[2][j] + m_mat[i][3] * matrix.m[3][j];
			}
		}

		setMatrix(output);
	}

	void setMatrix(const Matrix4x4& matrix)
	{
		::memcpy(m_mat, matrix.m_mat, sizeof(float) * 16);
	}

	void setMatrix(const Vec3& x, const Vec3& y, const Vec3& z)
	{
		m_mat[0][0] = x.x; 
		m_mat[0][1] = x.y;
		m_mat[0][2] = x.z;

		m_mat[1][0] = y.x;
		m_mat[1][1] = y.y;
		m_mat[1][2] = y.z;

		m_mat[2][0] = z.x;
		m_mat[2][1] = z.y;
		m_mat[2][2] = z.z;
	}

	Vec3 getZDirection()
	{
		return Vec3(m_mat[2][0], m_mat[2][1], m_mat[2][2]);
	}
	Vec3 getYDirection()
	{
		return Vec3(m_mat[1][0], m_mat[1][1], m_mat[1][2]);
	}
	Vec3 getXDirection()
	{
		return Vec3(m_mat[0][0], m_mat[0][1], m_mat[0][2]);
	}
	Vec3 getTranslation()
	{
		return Vec3(m_mat[3][0], m_mat[3][1], m_mat[3][2]);
	}

	void setPerspectiveFovLH(float fov, float aspect, float znear, float zfar)
	{
		float yscale = 1.0f / tan(fov/2.0f);
		float xscale = yscale / aspect;
		m_mat[0][0] = xscale;
		m_mat[1][1] = yscale;
		m_mat[2][2] = zfar / (zfar - znear);
		m_mat[2][3] = 1.0f;
		m_mat[3][2] = (-znear*zfar) / (zfar - znear);
	}

	//used for directional light shadow mapping
	void setOrthoLH(float width, float height, float near_plane, float far_plane)
	{
		setIdentity();
		m_mat[0][0] = 2.0f / width;
		m_mat[1][1] = 2.0f / height;
		m_mat[2][2] = 1.0f / (far_plane - near_plane);
		m_mat[3][2] = -(near_plane / (far_plane - near_plane));
	}

	float m_mat[4][4] = {};

};
