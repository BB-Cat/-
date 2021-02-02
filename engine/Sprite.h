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
	//load a sprite texture from a file path
	Sprite(const wchar_t* full_path);
	//create a sprite object from a preexisting texture
	Sprite(TexturePtr tex);
	~Sprite();
public:
	void renderWorldSpaceSprite(Vec3 scale, Vec3 position, Vec3 rotation, 
		Vec2 sprite_pos, Vec2 sprite_size, Vec2 origin);

	void renderScreenSpaceSprite(Vec2 scale, Vec2 screen_position, float rotation,
		Vec2 sprite_pos, Vec2 sprite_size, Vec2 origin);

	Matrix4x4 applyTransformations(const Matrix4x4& global, Vec3 scale, Vec3 rot, Vec3 translate);

	TexturePtr getTexture() { return m_tex; }


private:
	void init();

protected:
	TexturePtr m_tex;
	ID3D11Buffer* m_buffer;
	ID3D11InputLayout* m_layout;
	ID3D11DepthStencilState* m_dss;
	ID3D11RasterizerState* m_rs;

protected:
	friend class DeviceContext;
};
