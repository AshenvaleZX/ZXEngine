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
		matM_Inv = Math::Inverse(matM);
	}

	void RenderEngineProperties::SetShadowMap(uint32_t id, bool isBuffer)
	{
		shadowMap = id;
		isShadowMapBuffer = isBuffer;
	}

	void RenderEngineProperties::SetShadowCubeMap(uint32_t id, bool isBuffer)
	{
		shadowCubeMap = id;
		isShadowCubeMapBuffer = isBuffer;
	}
}