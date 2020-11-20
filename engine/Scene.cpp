#include "Scene.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "CameraManager.h"
#include "GBuffer.h"
#include "AppWindow.h"

void Scene::render(float delta, const float& width, const float& height)
{

	/* this needs to be changed. we only need one shadow map texture total for all 3 cascades combined. */
	//repeat the shadowRenderPass once for each shadowmap
	for (int i = 0; i < 3; i++)
	{
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRenderTargetShadowMap(i);
		CameraManager::get()->setDirectionalLightWVPBuffer(m_scene_light_dir, width, height, i);
		shadowRenderPass(delta);
	}


	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRenderTargetDirect();
	GraphicsEngine::get()->getRenderSystem()->getGBuffer()->setShadowMapTex();
	CameraManager::get()->setWorldBuffer();
	mainRenderPass(delta);

	addRenderPass(delta);
}
