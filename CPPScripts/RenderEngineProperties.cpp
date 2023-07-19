#include "RenderEngineProperties.h"
#include "Component/Light.h"
#include "Component/ZCamera.h"
#include "Component/Transform.h"
#include "Component/MeshRenderer.h"

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

	void RenderEngineProperties::SetLightProperties(const vector<Light*>& lights)
	{
		if (lights.empty())
			return;

		lightPos = lights[0]->GetTransform()->GetPosition();
		lightDir = lights[0]->GetTransform()->GetForward();
		lightColor = lights[0]->color;
		lightIntensity = lights[0]->intensity;
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