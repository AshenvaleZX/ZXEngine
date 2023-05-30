#include "RenderPassRayTracing.h"
#include "RenderAPI.h"
#include "RenderQueueManager.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "MaterialData.h"
#include "ZCamera.h"
#include "Light.h"
#include "FBOManager.h"
#include "GlobalData.h"

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

		// �л�����FBO
		FBOManager::GetInstance()->SwitchFBO("Main");
		// ViewPort����Ϊ���ڴ�С
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);

		// ������Ⱦ���У�������mesh��VAO�Ͳ������������׷����
		for (auto renderer : renderQueue->GetRenderers())
		{
			renderAPI->PushRayTracingMaterialData(renderer->matetrial->data);
			for (auto mesh : renderer->meshes)
			{
				renderAPI->PushAccelerationStructure(mesh->VAO, renderer->matetrial->data->GetRTID(), renderer->GetTransform()->GetModelMatrix());
			}
		}

		// ����TLAS
		renderAPI->BuildTopLevelAccelerationStructure(asCommandID);

		// ��׷���߳�������
		rtConstants.VP = camera->GetProjectionMatrix() * camera->GetViewMatrix();
		rtConstants.V_Inv = camera->GetViewMatrixInverse();
		rtConstants.P_Inv = camera->GetProjectionMatrixInverse();
		rtConstants.lightPos = Light::GetAllLights()[0]->GetTransform()->GetPosition();

		// ��׷��Ⱦ
		renderAPI->RayTrace(rtCommandID, rtConstants);

		// ������Ⱦ����
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}
}