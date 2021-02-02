#pragma once
#include "ConstantBufferFormats.h"
#include "Prerequisites.h"



//class which handles updating of constant buffers in the vertex and pixel shaders
class ConstantBufferSystem
{
public:
	ConstantBufferSystem();
	~ConstantBufferSystem();

	//create all critical buffers
	void initializeConstantBuffers();

	//update the constant buffer for local light properties
	void updateLocalLightPropertyBuffer(Vec3 pos, Vec3 color, float strength);
	//update the constant buffer for global light properties
	void updateGlobalLightPropertyBuffer(Vec3 dir, float strength, Vector4D color, Vector4D ambient_color);
	//update the constant buffer for hull shader
	void updateHullShaderBuffer(int tesselation_factor);

	//set the constant buffer for local light properties
	void setLocalLightPropertyBuffer();
	//set the constant buffer for global light properties
	void setGlobalLightPropertyBuffer();
	//set the constant buffer for hull shader
	void setHullShaderBuffer();

	//set the model transform properties for the next 3D mesh that will be rendered.
	void updateAndSetTransformationBuffer(const Matrix4x4& matrix, const Matrix4x4 bone_matrixes[MAXBONES]);
	//set the light properties for the next 3D mesh that will be rendered.
	void updateAndSetObjectLightPropertyBuffer(const Material_Obj& mat);
	//set the constant buffer for terrain color
	void updateAndSetTerrainColorBuffer(const Vector4D& c1, const Vector4D& c2, const Vector4D& c3);
	//set the time buffer.
	void updateAndSetTimeBuffer(const cb_time& t);
	//set the height buffer for DS
	void updateAndSetDSHeightBuffer(const float& height);

	void updateAndSetVSTesselationBuffer(const cb_tess& t);

	void updateAndSetPSNoiseBuffer(const cb_noise& n);
	void updateAndSetCSNoiseBuffer(const cb_noise& n);

	void updateAndSetPSCloudBuffer(const cb_cloud& c);

	void updateAndSetCSRaymarchBuffer(const cb_compute_raymarch& r);


private:
	//create the constant buffer for rotation properties 
	void createGlobalBuffer();
	//create world buffer for camera projection
	void createWorldBuffer();
	//create the constant buffer for global light properties 
	void createObjectLightPropertyBuffer();
	//create the constant buffer for local light properties
	void createLocalLightPropertyBuffer();
	//create the constant buffer for local light properties
	void createGlobalLightPropertyBuffer();
	//create the constant buffer for terrain color information
	void createTerrainColorBuffer();
	//create the constant buffer for tesselation settings in the hull shader
	void createHullShaderBuffer();
	//create the constant buffer for time to calculate cyclical movements
	void createTimeBuffer();
	//create the constant buffer for domain shader heightmap settings
	void createDSHeightBuffer();

	void createTessBuffer();
	//create the constant buffer for noise creation
	void createNoiseBuffer();
	//create the constant buffer for cloud properties
	void createCloudBuffer();
	//create the constant buffer for the raymarch compute shader
	void createRaymarchBuffer();

private:
	//this buffer holds the lighting properties for meshes and is overwritten every time a mesh's render function is called.
	MyConstantBufferPtr m_light_properties_cb;
	//this constant buffer holds data for point lights in the scene
	MyConstantBufferPtr m_local_light_cb;
	//this constant buffer holds the global lighting information for the scene
	MyConstantBufferPtr m_global_light_cb;
	//this pointer holds the transformation matrix updated each time a mesh is drawn
	MyConstantBufferPtr m_global_cb;
	//this pointer holds color information for rendering low resolution terrain chunks
	MyConstantBufferPtr m_terrain_color_cb;
	//this pointer holds the tesselation settings for the hull buffer
	MyConstantBufferPtr m_hull_cb;
	//this pointer holds the time information for relevant shaders
	MyConstantBufferPtr m_time_cb;
	//constant buffer to determine height map settings for domain shader
	MyConstantBufferPtr m_DS_height_cb;

	MyConstantBufferPtr m_tess_cb;
	//constant buffer for modifying noise 
	MyConstantBufferPtr m_noise_cb;
	//constant buffer for cloud properties
	MyConstantBufferPtr m_cloud_cb;

	//constant buffer for the ray march compute shader
	MyConstantBufferPtr m_raymarch_cb;

private:
	//constant buffer information for lights.  this should be moved to a proper lighting class later on
	cb_lights m_l;
	//integer which tracks how many lights have been set since the last update.  
	//if it exceeds the maximum number of light sources supported, additional updates are ignored.
	int m_lights_set;
};