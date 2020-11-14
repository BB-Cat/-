#include "Matrix4X4.h"
#include "Prerequisites.h"
#include <vector>
#include "Subset.h"

//primitive cube class
class Cube
{
public:
	Cube(VertexBufferPtr vertexes, IndexBufferPtr indexes);
	~Cube();

	//render using the cube object's position, rotation and scale values
	void render(int shader, bool is_textured = true);
	//render with manually set position, rotation and scale values
	void render(Vector3D scale, Vector3D position, Vector3D rotation, int shader, bool is_textured = true);

	bool loadDiffuseTex(const wchar_t* file);
	bool loadNormalTex(const wchar_t* file);
	bool loadRoughnessTex(const wchar_t* file);

	void setPosition(Vector3D pos) { m_pos = pos; }
	void setScale(Vector3D scale) { m_scale = scale; }
	void setRotation(Vector3D rot) { m_rot = rot; }
	void setMaterial(Material_Obj mat) { m_mat = mat; }

	Vector3D getPosition() { return m_pos; }
	Vector3D getScale() { return m_scale; }
	Vector3D getRotation() { return m_rot; }
	Material_Obj getMaterial() { return m_mat; }

private:
	Matrix4x4 applyTransformations(const Matrix4x4& global, Vector3D scale, Vector3D rot, Vector3D translate);

	VertexBufferPtr		m_vertex_buffer;
	IndexBufferPtr		m_index_buffer;
	Matrix4x4			m_mesh_world;

	TexturePtr			m_diffuse;
	TexturePtr			m_normal;
	TexturePtr			m_roughness;

	Material_Obj		m_mat;
	Vector3D			m_pos;
	Vector3D			m_scale;
	Vector3D			m_rot;

	//temporary
	ID3D11RasterizerState* m_solid_rast;
};