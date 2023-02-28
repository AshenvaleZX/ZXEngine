#include "RenderEngineProperties.h"
#include "Light.h"
#include "ZCamera.h"
#include "Transform.h"
#include "MeshRenderer.h"

namespace ZXEngine
{
	RenderEngineProperties* RenderEngineProperties::mInstance = nullptr;

	void RenderEngineProperties::Create()
	{
		mInstance = new RenderEngineProperties();
	}

	RenderEngineProperties* RenderEngineProperties::GetInstance()
	{
		return mInstance;
	}

	void RenderEngineProperties::SetLightProperties(Light* light)
	{
		lightPos = light->GetTransform()->GetPosition();
		lightDir = light->GetTransform()->GetForward();
		lightColor = light->color;
		lightIntensity = light->intensity;
	}

	void RenderEngineProperties::SetCameraProperties(Camera* camera)
	{
		matV = camera->GetViewMatrix();
		matP = camera->GetProjectionMatrix();
		camPos = camera->GetTransform()->GetPosition();
	}

	void RenderEngineProperties::SetRendererProperties(MeshRenderer* renderer)
	{
		matM = renderer->GetTransform()->GetModelMatrix();
	}

	void RenderEngineProperties::SetShadowMap(unsigned int id)
	{
		shadowMap = id;
	}

	void RenderEngineProperties::SetShadowCubeMap(unsigned int id)
	{
		shadowCubeMap = id;
	}
}