#pragma once
#include "Matrix4X4.h"
#include "Prerequisites.h"
#include <vector>
#include "Subset.h"

class Primitive
{
public:
	Primitive(VertexBufferPtr vertexes, IndexBufferPtr indexes);
	~Primitive();

	//render using the cube object's position, rotation and scale values
	/* (Set the shader to -1 to use the cube's internal shader setting - Lambert by default) */
	//void render(int shader = -1, bool is_textured = true);
	//render with manually set position, rotation and scale values
	/* (Set the shader to -1 to use the cube's internal shader setting - Lambert by default) */
	void render(Vec3 scale, Vec3 position, Vec3 rotation, int shader, bool is_textured = true);

	bool loadDiffuseTex(const wchar_t* file);
	bool loadNormalTex(const wchar_t* file);
	bool loadRoughnessTex(const wchar_t* file);

	bool loadDiffuseTex(TexturePtr tex, std::string name);
	bool loadNormalTex(TexturePtr tex, std::string name);
	bool loadRoughnessTex(TexturePtr tex, std::string name);

	bool fetchDiffuseTex(std::string name);
	bool fetchNormalTex(std::string name);
	bool fetchRoughnessTex(std::string name);

	//void setPosition(Vector3D pos) { m_pos = pos; }
	//void setScale(Vector3D scale) { m_scale = scale; }
	//void setRotation(Vector3D rot) { m_rot = rot; }
	void setColor(Vec3 col);
	void setTransparency(float t);
	void setMaterial(Material_Obj mat) { m_mat = mat; }
	//void setShader(int shader) { m_shader = shader; }

	//Vector3D getPosition() { return m_pos; }
	//Vector3D getScale() { return m_scale; }
	//Vector3D getRotation() { return m_rot; }
	Material_Obj getMaterial() { return m_mat; }
	//int getShader() { return m_shader; }

	std::string getDiffuseName() { return m_diffuse_name; }
	std::string getNormalName() { return m_normal_name; }
	std::string getRoughnessName() { return m_roughness_name; }

private:
	Matrix4x4 applyTransformations(const Matrix4x4& global, Vec3 scale, Vec3 rot, Vec3 translate);

private:
	VertexBufferPtr		m_vertex_buffer;
	IndexBufferPtr		m_index_buffer;
	Matrix4x4			m_mesh_world;
	//std::vector<Mesh_Data> m_meshdata;

	TexturePtr			m_diffuse;
	std::string			m_diffuse_name = "none";
	TexturePtr			m_normal;
	std::string			m_normal_name = "none";
	TexturePtr			m_roughness;
	std::string			m_roughness_name = "none";

	Material_Obj		m_mat;
	//Vector3D			m_pos;
	//Vector3D			m_scale;
	//Vector3D			m_rot;

	//int					m_shader;

	//temporary
	ID3D11RasterizerState* m_solid_rast;
};