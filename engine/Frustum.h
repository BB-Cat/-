#pragma once
#include "Matrix4X4.h"
#include <DirectXMath.h>

/* 
This class is used to generate a view frustum from the current camera data, 
which is then used to pick which terrain chunks to cull.  A frustum will be generated once every
frame inside the camera manager.
*/

class Frustum
{
public:
	Frustum() {}
	~Frustum() {}

	void constructFrustum(float screen_depth, Matrix4x4 proj, Matrix4x4 view)
	{
		float zMinimum, r;
		Matrix4x4 matrix;

		float a, b, c, d;


		// Calculate the minimum Z distance in the frustum.
		//zMinimum = -proj._43 / proj._33;
		zMinimum = -proj.m_mat[3][2] / proj.m_mat[2][2];
		r = screen_depth / (screen_depth - zMinimum);
		//proj._33 = r;
		//proj._43 = -r * zMinimum;
		proj.m_mat[2][2] = r;
		proj.m_mat[3][2] = -r * zMinimum;

		// Create the frustum matrix from the view matrix and updated projection matrix.
		//D3DXMatrixMultiply(&matrix, &view, &proj);

		matrix = view * proj;

		//// Calculate near plane of frustum.
		//m_planes[0].a = matrix._14 + matrix._13;
		//m_planes[0].b = matrix._24 + matrix._23;
		//m_planes[0].c = matrix._34 + matrix._33;
		//m_planes[0].d = matrix._44 + matrix._43;
		//D3DXPlaneNormalize(&m_planes[0], &m_planes[0]);


		a = matrix.m_mat[0][3] + matrix.m_mat[0][2];
		b = matrix.m_mat[1][3] + matrix.m_mat[1][2];
		c = matrix.m_mat[2][3] + matrix.m_mat[2][2];
		d = matrix.m_mat[3][3] + matrix.m_mat[3][2];

		m_planes[0] = {a, b, c, d};
		m_planes[0] = DirectX::XMPlaneNormalize(m_planes[0]);

		//// Calculate far plane of frustum.
		//a = matrix._14 - matrix._13;
		//b = matrix._24 - matrix._23;
		//c = matrix._34 - matrix._33;
		//d = matrix._44 - matrix._43;

		a = matrix.m_mat[0][3] - matrix.m_mat[0][2];
		b = matrix.m_mat[1][3] - matrix.m_mat[1][2];
		c = matrix.m_mat[2][3] - matrix.m_mat[2][2];
		d = matrix.m_mat[3][3] - matrix.m_mat[3][2];

		m_planes[1] = { a, b, c, d };
		//D3DXPlaneNormalize(&m_planes[1], &m_planes[1]);
		m_planes[1] = DirectX::XMPlaneNormalize(m_planes[1]);



		// Calculate left plane of frustum.
		//m_planes[2].a = matrix._14 + matrix._11;
		//m_planes[2].b = matrix._24 + matrix._21;
		//m_planes[2].c = matrix._34 + matrix._31;
		//m_planes[2].d = matrix._44 + matrix._41;

		a = matrix.m_mat[0][3] + matrix.m_mat[0][0];
		b = matrix.m_mat[1][3] + matrix.m_mat[1][0];
		c = matrix.m_mat[2][3] + matrix.m_mat[2][0];
		d = matrix.m_mat[3][3] + matrix.m_mat[3][0];

		m_planes[2] = { a, b, c, d };
		//D3DXPlaneNormalize(&m_planes[1], &m_planes[1]);
		m_planes[2] = DirectX::XMPlaneNormalize(m_planes[2]);

		// Calculate right plane of frustum.
		//m_planes[3].a = matrix._14 - matrix._11;
		//m_planes[3].b = matrix._24 - matrix._21;
		//m_planes[3].c = matrix._34 - matrix._31;
		//m_planes[3].d = matrix._44 - matrix._41;

		a = matrix.m_mat[0][3] - matrix.m_mat[0][0];
		b = matrix.m_mat[1][3] - matrix.m_mat[1][0];
		c = matrix.m_mat[2][3] - matrix.m_mat[2][0];
		d = matrix.m_mat[3][3] - matrix.m_mat[3][0];

		m_planes[3] = { a, b, c, d };
		//D3DXPlaneNormalize(&m_planes[1], &m_planes[1]);
		m_planes[3] = DirectX::XMPlaneNormalize(m_planes[3]);

		// Calculate top plane of frustum.
		//m_planes[4].a = matrix._14 - matrix._12;
		//m_planes[4].b = matrix._24 - matrix._22;
		//m_planes[4].c = matrix._34 - matrix._32;
		//m_planes[4].d = matrix._44 - matrix._42;

		a = matrix.m_mat[0][3] - matrix.m_mat[0][1];
		b = matrix.m_mat[1][3] - matrix.m_mat[1][1];
		c = matrix.m_mat[2][3] - matrix.m_mat[2][1];
		d = matrix.m_mat[3][3] - matrix.m_mat[3][1];

		m_planes[4] = { a, b, c, d };
		//D3DXPlaneNormalize(&m_planes[1], &m_planes[1]);
		m_planes[4] = DirectX::XMPlaneNormalize(m_planes[4]);

		// Calculate bottom plane of frustum.
		//m_planes[5].a = matrix._14 + matrix._12;
		//m_planes[5].b = matrix._24 + matrix._22;
		//m_planes[5].c = matrix._34 + matrix._32;
		//m_planes[5].d = matrix._44 + matrix._42;

		a = matrix.m_mat[0][3] + matrix.m_mat[0][1];
		b = matrix.m_mat[1][3] + matrix.m_mat[1][1];
		c = matrix.m_mat[2][3] + matrix.m_mat[2][1];
		d = matrix.m_mat[3][3] + matrix.m_mat[3][1];

		m_planes[5] = { a, b, c, d };
		//D3DXPlaneNormalize(&m_planes[1], &m_planes[1]);
		m_planes[5] = DirectX::XMPlaneNormalize(m_planes[5]);

		return;
	}

	bool Frustum::checkPoint(Vector3D v)
	{
		DirectX::XMVECTOR p = { v.m_x, v.m_y, v.m_z };
		DirectX::XMFLOAT4 res;


		// Check if the point is inside all six planes of the view frustum.
		for (int i = 0; i < 6; i++)
		{
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], p));
			if (res.x < 0.0f) return false;
		}

		return true;
	}

	bool checkCube(Vector3D center, float radius)
	{
		//for (int i = 0; i < 8; i++)
		//{
		//	if (checkPoint(v[i])) return true;
		//}

		DirectX::XMVECTOR c = { center.m_x, center.m_y, center.m_z };
		DirectX::XMVECTOR check;
		DirectX::XMFLOAT4 res;

		// Check if any one point of the cube is in the view frustum.
		for (int i = 0; i < 6; i++)
		{
			check = {center.m_x - radius, center.m_y - radius, center.m_z - radius};
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], check));
			if (res.x >= 0.0f) continue;
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		//{
		//	continue;
		//}

			check = { center.m_x + radius, center.m_y - radius, center.m_z - radius };
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], check));
			if (res.x >= 0.0f) continue;
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		//{
		//	continue;
		//}

			check = { center.m_x - radius, center.m_y + radius, center.m_z - radius };
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], check));
			if (res.x >= 0.0f) continue;
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		//{
		//	continue;
		//}

			check = { center.m_x + radius, center.m_y + radius, center.m_z - radius };
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], check));
			if (res.x >= 0.0f) continue;
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		//{
		//	continue;
		//}

			check = { center.m_x - radius, center.m_y - radius, center.m_z + radius };
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], check));
			if (res.x >= 0.0f) continue;
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		//{
		//	continue;
		//}

			check = { center.m_x + radius, center.m_y - radius, center.m_z + radius };
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], check));
			if (res.x >= 0.0f) continue;
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		//{
		//	continue;
		//}

			check = { center.m_x - radius, center.m_y + radius, center.m_z + radius };
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], check));
			if (res.x >= 0.0f) continue;
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		//{
		//	continue;
		//}

			check = { center.m_x + radius, center.m_y + radius, center.m_z + radius };
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], check));
			if (res.x >= 0.0f) continue;
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		//{
		//	continue;
		//}
			return false;
		}

		return true;
	}

	bool checkSphere(Vector3D v, float radius)
	{
		DirectX::XMVECTOR p = { v.m_x, v.m_y, v.m_z };
		DirectX::XMFLOAT4 res;

		// Check if the radius of the sphere is inside the view frustum.
		for (int i = 0; i < 6; i++)
		{
			DirectX::XMStoreFloat4(&res, DirectX::XMPlaneDotCoord(m_planes[i], p));
			if (res.x < -radius) return false;
		}
		return true;
	}

private:
	DirectX::XMVECTOR m_planes[6];
};