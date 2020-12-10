#include "Matrix4X4.h"
#include "Prerequisites.h"
#include <vector>
#include "Subset.h"
#include "Colliders.h"

namespace ObjectType
{
	enum 
	{
		Mesh,
		Primitive
	};
}

class WorldObject
{
public:
	//WorldObject(VertexBufferPtr vertexes, IndexBufferPtr indexes, Collider* collider = nullptr);
	WorldObject(PrimitivePtr primitive, int collider_type, 
		Vec3 pos = Vec3(0,0,0), Vec3 scale = Vec3(1,1,1), Vec3 rot = Vec3(0,0,0), int shader = Shaders::LAMBERT);
	WorldObject(SkinnedMeshPtr mesh, Collider* collider = nullptr,
		Vec3 pos = Vec3(0, 0, 0), Vec3 scale = Vec3(1, 1, 1), Vec3 rot = Vec3(0, 0, 0), int shader = Shaders::LAMBERT);
	~WorldObject();

	//render using the cube object's position, rotation and scale values
	/* (Set the shader to -1 to use the cube's internal shader setting - Lambert by default) */
	void render(float elapsed_time, int shader = -1, bool is_textured = true);
	//render with manually set position, rotation and scale values
	/* (Set the shader to -1 to use the cube's internal shader setting - Lambert by default) */
	void render(float elapsed_time, Vec3 scale, Vec3 position, Vec3 rotation, int shader = -1, bool is_textured = true);

	//bool loadDiffuseTex(const wchar_t* file);
	//bool loadNormalTex(const wchar_t* file);
	//bool loadRoughnessTex(const wchar_t* file);

	//bool loadDiffuseTex(TexturePtr tex, std::string name);
	//bool loadNormalTex(TexturePtr tex, std::string name);
	//bool loadRoughnessTex(TexturePtr tex, std::string name);

	//bool fetchDiffuseTex(std::string name);
	//bool fetchNormalTex(std::string name);
	//bool fetchRoughnessTex(std::string name);


	void setPosition(Vec3 pos) { m_pos = pos; }
	void setScale(Vec3 scale) { m_scale = scale; }
	void setRotation(Vec3 rot) { m_rot = rot; }
	//void setMaterial(Material_Obj mat) { m_mat = mat; }
	void setShader(int shader) { m_shader = shader; }
	void setCollider(Collider* collider) { m_collider = collider; }
	void setMaterial(Material_Obj mat);
	void setPrefabName(std::string name) { m_prefab_name = name; }

	Vec3 getPosition() { return m_pos; }
	Vec3 getScale() { return m_scale; }
	Vec3 getRotation() { return m_rot; }
	int getShader() { return m_shader; }
	Collider* getCollider() { return m_collider; }
	Material_Obj getMaterial();

	int getObjectType() { return m_object_type; }
	PrimitivePtr getPrimitive() { return m_primitive; }
	SkinnedMeshPtr getMesh() { return m_mesh; }
	std::string getPrefabName() { return m_prefab_name; }

	void toggleBoundingBoxRender() { m_render_bounding_box = !m_render_bounding_box; }
	bool getBoundingBoxRenderState() { return m_render_bounding_box; }



private:
	Matrix4x4 applyTransformations(const Matrix4x4& global, Vec3 scale, Vec3 rot, Vec3 translate);

private:
	PrimitivePtr m_primitive = nullptr;
	SkinnedMeshPtr m_mesh = nullptr;
	int m_object_type;

	//VertexBufferPtr		m_vertex_buffer;
	//IndexBufferPtr		m_index_buffer;
	//Matrix4x4			m_mesh_world;
	//std::vector<Mesh_Data> m_meshdata;

	//TexturePtr			m_diffuse;
	//std::string			m_diffuse_name = "none";
	//TexturePtr			m_normal;
	//std::string			m_normal_name = "none";
	//TexturePtr			m_roughness;
	//std::string			m_roughness_name = "none";

	//Material_Obj		m_mat;
	Vec3			m_pos;
	Vec3			m_scale;
	Vec3			m_rot;
	Collider*			m_collider = nullptr;
	bool				m_render_bounding_box = false;

	std::string m_prefab_name = "none";

	int					m_shader;

	//temporary
	//ID3D11RasterizerState* m_solid_rast;
};