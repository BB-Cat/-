#include "ConstantBufferSystem.h"
#include "MyConstantBuffer.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include <d3dcompiler.h>
#include "Subset.h"
#include <DirectXMath.h>
#include "CameraManager.h"

ConstantBufferSystem::ConstantBufferSystem()
{
}

ConstantBufferSystem::~ConstantBufferSystem()
{
}

void ConstantBufferSystem::initializeConstantBuffers()
{
	createObjectLightPropertyBuffer();
	createGlobalLightPropertyBuffer();
	createLocalLightPropertyBuffer();
	createGlobalBuffer();
	createWorldBuffer();
	createHullShaderBuffer();
	createTimeBuffer();
	createDSHeightBuffer();
	createTerrainColorBuffer();
	createTessBuffer();
	createNoiseBuffer();
	createCloudBuffer();
}

void ConstantBufferSystem::createGlobalBuffer()
{
	cb_local g;
	m_global_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&g, sizeof(cb_local));
}

void ConstantBufferSystem::createWorldBuffer()
{
	CameraManager::get()->createWorldBuffers();
}

void ConstantBufferSystem::createObjectLightPropertyBuffer()
{
	cb_mesh_light_properties lp;
	m_light_properties_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&lp, sizeof(cb_mesh_light_properties));
}

void ConstantBufferSystem::createLocalLightPropertyBuffer()
{
	//for (int i = 0; i < NUM_LIGHTS; i++)
	//{
	//	m_l.m_light_pos[i] = {};
	//	m_l.m_light_color[i] = {};
	//	m_l.m_light_strength[i] = {};
	//}

	m_local_light_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&m_l, sizeof(cb_lights));
}

void ConstantBufferSystem::createGlobalLightPropertyBuffer()
{
	cb_scene_lighting gl;

	gl.m_global_light_color = Vector3D(1.0f, 0.9f, 0.3f);
	gl.m_global_light_dir = Vector3D(0.05f, 1.0f, 0.05f);
	gl.m_global_light_strength = 1.0f;
	gl.m_ambient_light_color = Vector3D(0, 0, 0);

	m_global_light_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&gl, sizeof(cb_scene_lighting));
}

void ConstantBufferSystem::createTerrainColorBuffer()
{
	cb_vs_terrainLD t;
	t.m_c1 = Vector4D(1, 0, 0, 0);
	t.m_c2 = Vector4D(0, 1, 0, 0);
	t.m_c3 = Vector4D(0, 0, 1, 0);

	m_terrain_color_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&t, sizeof(cb_vs_terrainLD));
}

void ConstantBufferSystem::createHullShaderBuffer()
{
	cb_tess h;

	h.m_max_tess = 1;

	m_hull_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&h, sizeof(cb_tess));
}

void ConstantBufferSystem::createTimeBuffer()
{
	cb_time t;

	t.m_time = 0;
	t.m_elapsed = 0;

	m_time_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&t, sizeof(cb_tess));
}

void ConstantBufferSystem::createDSHeightBuffer()
{
	cb_ds_height h;
	h.m_height = 0.16f;

	m_DS_height_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&h, sizeof(cb_ds_height));
}

void ConstantBufferSystem::createTessBuffer()
{
	cb_tess t;
	t.m_max_tess = 8;
	t.m_min_tess = 1;
	t.m_min_tess_range = 200.0f;
	t.m_max_tess_range = 120.0f;

	//	static const float MinTess = 1;
	//static const float MaxTess = 8;
	//static const float MinTessRange = 200.0f;
	//static const float MaxTessRange = 120.0f;

	m_tess_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&t, sizeof(cb_tess));
}

void ConstantBufferSystem::createNoiseBuffer()
{
	cb_noise n;
	n.m_noise_type = Vector4D(0, 0, 0, 1);
	n.m_show_rgba = Vector4D(1, 0, 0, 0);
	
	n.m_vor_amplitude = 1.0f;
	n.m_vor_frequency = 4.0f;
	n.m_vor_gain = 0.3f;
	n.m_vor_lacunarity = 2.0f;
	n.m_vor_octaves = 1;
	n.m_vor_cell_size = 0.25f;
	
	n.m_per_amplitude = 1.0f;
	n.m_per_frequency = 4.0f;
	n.m_per_gain = 0.3f;
	n.m_per_lacunarity = 2.0f;
	n.m_per_octaves = 1;
	n.m_per_cell_size = 0.25f;
	
	m_noise_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&n, sizeof(cb_noise));
}

void ConstantBufferSystem::createCloudBuffer()
{
	cb_cloud c;
	c.m_cloud_density = 0.06f;
	c.m_cloud_position = {};
	c.m_cloud_size = {};
	c.m_per_pixel_fade_threshhold = {};
	c.m_per_sample_fade_threshhold = {};
	c.m_horizontal_fade = 0.6f;
	c.m_vertical_fade = 0.6f;
	c.m_in_scattering_strength = 0.0f;
	c.m_out_scattering_strength = 0.0f;
	c.m_move_dir = {};
	c.m_sampling_resolution = Vector4D(40.0f, 40.0f, 40.0f, 40.0f);
	c.m_sampling_weight = Vector4D(0.4, 0.3, 0.2, 0.1);
	c.m_time = 0;
	c.m_speed = 0;

	m_cloud_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&c, sizeof(cb_cloud));

}

void ConstantBufferSystem::updateLocalLightPropertyBuffer(Vector3D pos, Vector3D color, float strength)
{
	if (m_lights_set < NUM_LIGHTS) 
	{
		m_l.m_light_pos[m_lights_set] = pos;
		m_l.m_light_color[m_lights_set] = color;
		m_l.m_light_strength[m_lights_set].m_x = strength;

		m_lights_set++;
	}
	m_local_light_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &m_l);
}

void ConstantBufferSystem::updateGlobalLightPropertyBuffer(Vector3D dir, float strength, Vector4D color, Vector4D ambient_color)
{
	cb_scene_lighting gl;
	gl.m_global_light_dir = dir;
	gl.m_global_light_strength = strength;
	gl.m_global_light_color = color;
	gl.m_ambient_light_color = ambient_color;

	m_global_light_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &gl);
}

void ConstantBufferSystem::updateHullShaderBuffer(int tesselation_factor)
{
	cb_tess h;

	h.m_max_tess = tesselation_factor;

	m_hull_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &h);
}

void ConstantBufferSystem::setLocalLightPropertyBuffer()
{

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBufferLightingPS(m_local_light_cb);

	//reset the light counter
	m_lights_set = 0;
	//clear the light data for the next frame
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
			m_l.m_light_color[i] = {};
			m_l.m_light_pos[i] = {};
			m_l.m_light_strength[i] = {};
	}
}

void ConstantBufferSystem::setGlobalLightPropertyBuffer()
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBufferSceneLightingPS(m_global_light_cb);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBufferSceneLightingVS(m_global_light_cb);
}

void ConstantBufferSystem::setHullShaderBuffer()
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantHullBufferHS(m_hull_cb);
}

void ConstantBufferSystem::updateAndSetTransformationBuffer(const Matrix4x4& matrix, const Matrix4x4 bone_matrixes[MAXBONES])
{
	cb_local g;
	g.m_local = matrix;

	for (int i = 0; i < MAXBONES; i++)
	{
		g.m_bone_transforms[i] = bone_matrixes[i];
	}

	//update the constant light properties buffer
	m_global_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &g);

	//send the updated buffer to the vertex shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantTransformBufferVS(m_global_cb);
	//test - send the buffer to the geometry shader as well
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantTransformBufferGS(m_global_cb);
	//test - send the buffer to the domain shader as well
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantTransformBufferDS(m_global_cb);
}

void ConstantBufferSystem::updateAndSetObjectLightPropertyBuffer(const Material_Obj& mat)
{
	cb_mesh_light_properties lp;

	lp.m_shininess = mat.m_shininess;
	lp.m_rimPower = mat.m_rimPower;
	lp.m_transparency =  mat.m_d;
	lp.m_metallicAmount = mat.m_metallicAmount;
	lp.m_ambientColor = mat.m_ambientColor;
	lp.m_diffuseColor = mat.m_diffuseColor;
	lp.m_specularColor = mat.m_specularColor;
	lp.m_emitColor = mat.m_emitColor;
	lp.m_rimColor = mat.m_rimColor;

	//update the constant light properties buffer
	m_light_properties_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &lp);

	//send the updated buffer to the pixel shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantMeshPropertyBufferPS(m_light_properties_cb);
}

void ConstantBufferSystem::updateAndSetTerrainColorBuffer(const Vector4D& c1, const Vector4D& c2, const Vector4D& c3)
{
	cb_vs_terrainLD t;
	t.m_c1 = c1;
	t.m_c2 = c2;
	t.m_c3 = c3;

	//update the terrain color buffer
	m_terrain_color_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &t);

	//send the updated buffer to the vertex shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBufferTerrainColorVS(m_terrain_color_cb);
}

void ConstantBufferSystem::updateAndSetTimeBuffer(const cb_time& t)
{
	cb_time time;
	time.m_elapsed = t.m_elapsed;
	time.m_time = t.m_time;

	//update the constant light properties buffer
	m_time_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &time);

	//send the updated buffer to the pixel shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantTimeBuffer(m_time_cb);
}

void ConstantBufferSystem::updateAndSetDSHeightBuffer(const float& height)
{
	cb_ds_height h;
	h.m_height = height;

	//update the ds height properties buffer
	m_DS_height_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &h);

	//send the updated buffer to the pixel shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDSHeightBuffer(m_DS_height_cb);
}

void ConstantBufferSystem::updateAndSetVSTesselationBuffer(const cb_tess& t)
{
	cb_tess temp = t;

	//update the constant light properties buffer
	m_tess_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &temp);

	//send the updated buffer to the pixel shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVSTessBuffer(m_tess_cb);
}

void ConstantBufferSystem::updateAndSetPSNoiseBuffer(const cb_noise& n)
{
	cb_noise temp = n;

	//update the noise buffer
	m_noise_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &temp);

	//send the updated buffer to the pixel shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setNoiseBufferPS(m_noise_cb);
}

void ConstantBufferSystem::updateAndSetPSCloudBuffer(const cb_cloud& c)
{
	cb_cloud temp = c;

	//update the noise buffer
	m_cloud_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &temp);

	//send the updated buffer to the pixel shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setCloudBufferPS(m_cloud_cb);
}
