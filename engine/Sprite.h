#pragma once
#include "Resource.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Matrix4X4.h"

//sprite class which renders 2D textures in orthographic form.  inherited by the spriteboard class for 2D textures inside 3D space.
class Sprite :public Resource
{
public:
	Sprite(const wchar_t* full_path);
	~Sprite();
public:
	void renderSprite(Vector3D scale, Vector3D position, Vector3D rotation, 
		Vector2D texture_pos, Vector2D texture_size, Vector2D origin);

	Matrix4x4 applyTransformations(const Matrix4x4& global, Vector3D scale, Vector3D rot, Vector3D translate);

protected:
	TexturePtr m_tex;
	ID3D11Buffer* m_buffer;
	ID3D11InputLayout* m_layout;
	ID3D11DepthStencilState* m_dss;
	ID3D11RasterizerState* m_rs;

	Matrix4x4 applyTransformations(Vector2D scale, Vector2D rot, Vector2D translate);
protected:
	friend class DeviceContext;
};
