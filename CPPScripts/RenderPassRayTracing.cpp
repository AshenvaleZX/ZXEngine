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

		// 切换到主FBO
		FBOManager::GetInstance()->SwitchFBO("Main");
		// ViewPort设置为窗口大小
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);

		// 遍历渲染队列，将所有mesh的VAO和材质数据推入光追管线
		for (auto renderer : renderQueue->GetRenderers())
		{
			renderAPI->PushRayTracingMaterialData(renderer->matetrial->data);
			for (auto mesh : renderer->meshes)
			{
				renderAPI->PushAccelerationStructure(mesh->VAO, renderer->matetrial->data->GetRTID(), renderer->GetTransform()->GetModelMatrix());
			}
		}

		// 构建TLAS
		renderAPI->BuildTopLevelAccelerationStructure(asCommandID);

		// 光追管线常量数据
		rtConstants.VP = camera->GetProjectionMatrix() * camera->GetViewMatrix();
		rtConstants.V_Inv = camera->GetViewMatrixInverse();
		rtConstants.P_Inv = camera->GetProjectionMatrixInverse();
		rtConstants.lightPos = Light::GetAllLights()[0]->GetTransform()->GetPosition();

		// 光追渲染
		renderAPI->RayTrace(rtCommandID, rtConstants);

		// 清理渲染队列
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}
}