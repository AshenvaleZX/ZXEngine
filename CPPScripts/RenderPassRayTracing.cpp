#include "RenderPassRayTracing.h"
#include "RenderAPI.h"
#include "RenderQueueManager.h"
#include "Component/MeshRenderer.h"
#include "Component/Transform.h"
#include "MaterialData.h"
#include "Component/ZCamera.h"
#include "Component/Light.h"
#include "FBOManager.h"
#include "GlobalData.h"
#include "CubeMap.h"
#include "SceneManager.h"
#include "Material.h"
#include "StaticMesh.h"

namespace ZXEngine
{
	RenderPassRayTracing::RenderPassRayTracing()
	{
		auto renderAPI = RenderAPI::GetInstance();

		asCommandID = renderAPI->AllocateDrawCommand(CommandType::RayTracing);
		rtCommandID = renderAPI->AllocateDrawCommand(CommandType::RayTracing);

		ClearInfo clearInfo = {};
		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_COLOR_BIT;
		FBOManager::GetInstance()->CreateFBO("RayTracing", FrameBufferType::RayTracing, clearInfo);
	}

	void RenderPassRayTracing::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);

		// 切换到主FBO
		FBOManager::GetInstance()->SwitchFBO("RayTracing");
		// ViewPort设置为窗口大小
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);

		// 设置天空盒
		renderAPI->SetRayTracingSkyBox(SceneManager::GetInstance()->GetCurScene()->skyBox->GetID());

		// 遍历渲染队列，将所有mesh的VAO和材质数据推入光追管线
		for (auto renderer : renderQueue->GetRenderers())
		{
			renderAPI->PushRayTracingMaterialData(renderer->mMatetrial);
			for (auto mesh : renderer->mMeshes)
			{
				renderAPI->PushAccelerationStructure(mesh->VAO, renderer->mMatetrial->hitGroupIdx, renderer->mMatetrial->data->GetRTID(), renderer->GetTransform()->GetModelMatrix());
			}
		}

		// 构建TLAS
		renderAPI->BuildTopLevelAccelerationStructure(asCommandID);

		// 光追管线常量数据
		rtConstants.VP = camera->GetProjectionMatrix() * camera->GetViewMatrix();
		rtConstants.V_Inv = camera->GetViewMatrixInverse();
		rtConstants.P_Inv = camera->GetProjectionMatrixInverse();

		// 光源位置
		auto allLights = Light::GetAllLights();
		if (allLights.size() > 0)
			rtConstants.lightPos = Light::GetAllLights()[0]->GetTransform()->GetPosition();

		// 光追渲染
		renderAPI->RayTrace(rtCommandID, rtConstants);

		// 清理渲染队列
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}
}