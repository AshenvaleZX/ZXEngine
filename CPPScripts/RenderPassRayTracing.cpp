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
#include "Time.h"

namespace ZXEngine
{
	RenderPassRayTracing::RenderPassRayTracing()
	{
		auto renderAPI = RenderAPI::GetInstance();

		asCommandID = renderAPI->AllocateDrawCommand(CommandType::RayTracing, ZX_CLEAR_FRAME_BUFFER_NONE_BIT);
		rtCommandID = renderAPI->AllocateDrawCommand(CommandType::RayTracing, ZX_CLEAR_FRAME_BUFFER_COLOR_BIT);

		ClearInfo clearInfo = {};
		FBOManager::GetInstance()->CreateFBO("RayTracing", FrameBufferType::RayTracing, clearInfo);
	}

	void RenderPassRayTracing::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);

		// �л�����FBO
		FBOManager::GetInstance()->SwitchFBO("RayTracing");
		// ViewPort����Ϊ���ڴ�С
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);

		// ������պ�
		renderAPI->SetRayTracingSkyBox(SceneManager::GetInstance()->GetCurScene()->skyBox->GetID());

		// ������Ⱦ���У�������mesh��VAO�Ͳ������������׷����
		for (auto renderer : renderQueue->GetRenderers())
		{
			renderAPI->PushRayTracingMaterialData(renderer->mMatetrial);
			for (auto& mesh : renderer->mMeshes)
			{
				renderAPI->PushAccelerationStructure(mesh->VAO, renderer->mMatetrial->hitGroupIdx, renderer->mMatetrial->data->GetRTID(), renderer->GetTransform()->GetModelMatrix());
			}
		}

		// ����TLAS
		renderAPI->BuildTopLevelAccelerationStructure(asCommandID);

		// ��׷���߳�������
		rtConstants.VP = camera->GetProjectionMatrix() * camera->GetViewMatrix();
		rtConstants.V_Inv = camera->GetViewMatrixInverse();
		rtConstants.P_Inv = camera->GetProjectionMatrixInverse();
#ifdef ZX_EDITOR
		rtConstants.time = Time::curEditorTime;
#else
		rtConstants.time = Time::curTime;
#endif

		// ��Դλ��
		auto allLights = Light::GetAllLights();
		if (allLights.size() > 0)
			rtConstants.lightPos = Light::GetAllLights()[0]->GetTransform()->GetPosition();

		// ��׷��Ⱦ
		renderAPI->RayTrace(rtCommandID, rtConstants);

		// ������Ⱦ����
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}
}