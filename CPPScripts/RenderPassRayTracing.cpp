#include "RenderPassRayTracing.h"
#include "RenderAPI.h"
#include "RenderQueueManager.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "MaterialData.h"
#include "ZCamera.h"
#include "Light.h"

namespace ZXEngine
{
	RenderPassRayTracing::RenderPassRayTracing()
	{
		auto renderAPI = RenderAPI::GetInstance();

		renderAPI->CreateRayTracingPipeline();
		renderAPI->CreateShaderBindingTable();

		asCommandID = renderAPI->AllocateDrawCommand(CommandType::RayTracing);
		rtCommandID = renderAPI->AllocateDrawCommand(CommandType::RayTracing);
	}

	void RenderPassRayTracing::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Qpaque);

		for (auto renderer : renderQueue->GetRenderers())
		{
			renderAPI->PushRayTracingMaterialData(renderer->matetrial->data);
			for (auto mesh : renderer->meshes)
			{
				renderAPI->PushAccelerationStructure(mesh->VAO, renderer->matetrial->data->GetRTID(), renderer->GetTransform()->GetModelMatrix());
			}
		}

		renderAPI->BuildTopLevelAccelerationStructure(asCommandID);

		rtConstants.VP = camera->GetProjectionMatrix() * camera->GetViewMatrix();
		rtConstants.V_Inv = camera->GetViewMatrixInverse();
		rtConstants.P_Inv = camera->GetProjectionMatrixInverse();
		rtConstants.lightPos = Light::GetAllLights()[0]->GetTransform()->GetPosition();

		renderAPI->RayTrace(rtCommandID, rtConstants);
	}
}