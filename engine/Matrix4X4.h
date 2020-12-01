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

	void setTranslation(const Vector3D& translation)
	{
		m_mat[3][0] = translation.m_x;
		m_mat[3][1] = translation.m_y;
		m_mat[3][2] = translation.m_z;
	}

	void setScale(const Vector3D& scale)
	{
		m_mat[0][0] = scale.m_x;
		m_mat[1][1] = scale.m_y;
		m_mat[2][2] = scale.m_z;
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

	void lookAt(Vector3D target, Vector3D pos, Vector3D up)
	{
		Vector3D forward = target - pos;
		forward.normalize();
		Vector3D side = Vector3D::cross(forward, up);
		side.normalize();
		Vector3D upward = Vector3D::cross(forward, side);

		setIdentity();

		m_mat[0][0] = side.m_x;
		m_mat[0][1] = side.m_y;
		m_mat[0][2] = side.m_z;

		m_mat[1][0] = upward.m_x;
		m_mat[1][1] = upward.m_y;
		m_mat[1][2] = upward.m_z;

		m_mat[2][0] = forward.m_x;
		m_mat[2][1] = forward.m_y;
		m_mat[2][2] = forward.m_z;

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

	Vector3D operator *(const Vector3D& v3)
	{
		Vector3D output;

		output.m_x = v3.m_x * m_mat[0][0] + v3.m_y * m_mat[0][1] + v3.m_z * m_mat[0][2] + 1.0f * m_mat[0][3];
		output.m_y = v3.m_x * m_mat[1][0] + v3.m_y * m_mat[1][1] + v3.m_z * m_mat[1][2] + 1.0f * m_mat[1][3];
		output.m_z = v3.m_x * m_mat[2][0] + v3.m_y * m_mat[2][1] + v3.m_z * m_mat[2][2] + 1.0f * m_mat[2][3];
		
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

	Vector3D getZDirection()
	{
		return Vector3D(m_mat[2][0], m_mat[2][1], m_mat[2][2]);
	}
	Vector3D getYDirection()
	{
		return Vector3D(m_mat[1][0], m_mat[1][1], m_mat[1][2]);
	}
	Vector3D getXDirection()
	{
		return Vector3D(m_mat[0][0], m_mat[0][1], m_mat[0][2]);
	}
	Vector3D getTranslation()
	{
		return Vector3D(m_mat[3][0], m_mat[3][1], m_mat[3][2]);
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
