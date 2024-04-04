#include "RenderPassGBufferGeneration.h"
#include "RenderAPI.h"
#include "FBOManager.h"
#include "ZShader.h"
#include "Resources.h"
#include "GlobalData.h"
#include "RenderQueueManager.h"
#include "Component/MeshRenderer.h"
#include "Material.h"
#include "MaterialData.h"
#include "Texture.h"
#include "RenderEngineProperties.h"
#include "RenderStateSetting.h"

namespace ZXEngine
{
	RenderPassGBufferGeneration::RenderPassGBufferGeneration()
	{
		mGBufferShader = new Shader(Resources::GetAssetFullPath("Shaders/GBufferGeneration.zxshader", true), FrameBufferType::GBuffer);
		mDrawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::GBufferGeneration);

		gBufferRenderState = new RenderStateSetting();

		ClearInfo clearInfo = {};
		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT;
		FBOManager::GetInstance()->CreateFBO("GBuffer", FrameBufferType::GBuffer, clearInfo);
	}

	void RenderPassGBufferGeneration::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		auto engineProperties = RenderEngineProperties::GetInstance();

		FBOManager::GetInstance()->SwitchFBO("GBuffer");
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);
		renderAPI->ClearFrameBuffer();

		engineProperties->SetCameraProperties(camera);

		renderAPI->SetRenderState(gBufferRenderState);

		// 延迟渲染仅支持不透明物体
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Deferred);
		renderQueue->Sort(camera, RenderSortType::FrontToBack);

		for (auto renderer : renderQueue->GetRenderers())
		{
			if (renderer->mGBufferMaterial == nullptr)
			{
				renderer->mGBufferMaterial = new Material(mGBufferShader);

				auto albedoMap = renderer->mMatetrial->data->GetTexture("_AlbedoMap");
				if (albedoMap)
					renderer->mGBufferMaterial->SetTexture("_AlbedoMap", albedoMap->GetID(), 0);

				auto normalMap = renderer->mMatetrial->data->GetTexture("_NormalMap");
				if (normalMap)
					renderer->mGBufferMaterial->SetTexture("_NormalMap", normalMap->GetID(), 1);
			}

			engineProperties->SetRendererProperties(renderer);

			renderer->mGBufferMaterial->SetEngineProperties();

			renderer->mGBufferMaterial->Use();

			renderer->Draw();
		}

		renderAPI->GenerateDrawCommand(mDrawCommandID);
	}
}