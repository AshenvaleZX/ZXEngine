#include "RenderPassDeferredRendering.h"
#include "RenderAPI.h"
#include "FBOManager.h"
#include "GlobalData.h"
#include "RenderQueueManager.h"
#include "RenderEngineProperties.h"
#include "Component/MeshRenderer.h"
#include "Material.h"
#include "MaterialData.h"
#include "ZShader.h"
#include "GeometryGenerator.h"
#include "Component/Light.h"
#include "Component/Transform.h"
#include "StaticMesh.h"
#include "RenderStateSetting.h"
#include "Component/ZCamera.h"
#include "SceneManager.h"
#include "CubeMap.h"

namespace ZXEngine
{
	RenderPassDeferredRendering::RenderPassDeferredRendering()
	{
		mBlitCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::NotCare, ZX_CLEAR_FRAME_BUFFER_NONE_BIT);
		mDrawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::DeferredRendering, ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);
		mScreenQuad = GeometryGenerator::CreateScreenQuad();

		skyBox = GeometryGenerator::CreateSimpleInwardBox();
		skyBoxMaterial = new Material(new Shader(Resources::GetAssetFullPath("Shaders/SkyBox.zxshader", true), FrameBufferType::Normal));

		skyBoxRenderState = new RenderStateSetting();
		skyBoxRenderState->depthTest = false;
		skyBoxRenderState->depthWrite = false;

		mDeferredShader = new Shader(Resources::GetAssetFullPath("Shaders/DeferredRender.zxshader", true), FrameBufferType::Deferred);
		mDeferredMaterial = new Material(mDeferredShader);

		opaqueRenderState = new RenderStateSetting();

		transparentRenderState = new RenderStateSetting();
		transparentRenderState->depthWrite = false;

		deferredRenderState = new RenderStateSetting();
		deferredRenderState->depthWrite = false;

		ClearInfo clearInfo = {};
		FBOManager::GetInstance()->CreateFBO("Deferred", FrameBufferType::Deferred, clearInfo);
	}

	void RenderPassDeferredRendering::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		FBOManager::GetInstance()->SwitchFBO("Deferred");
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);
		renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);

		// 复制 Depth Buffer
		renderAPI->BlitFrameBuffer(mBlitCommandID, "GBuffer", "Deferred", ZX_FRAME_BUFFER_PIECE_DEPTH);

		// 渲染天空盒
		renderAPI->SetRenderState(skyBoxRenderState);
		RenderSkyBox(camera);

		mDeferredMaterial->Use();

		// Light
		const auto& allLights = Light::GetAllLights();
		mDeferredMaterial->SetScalar("_LightNum", static_cast<int32_t>(allLights.size()));

		vector<Vector4> lightPos;
		vector<Vector4> lightColor;
		for (auto light : allLights)
		{
			lightPos.push_back(light->GetTransform()->GetPosition());
			lightColor.push_back(light->color);
		}
		mDeferredMaterial->SetVector("_LightPositions", lightPos.data(), static_cast<uint32_t>(lightPos.size()));
		mDeferredMaterial->SetVector("_LightColors", lightColor.data(), static_cast<uint32_t>(lightColor.size()));

		// G-Buffer
		auto gBuffer = FBOManager::GetInstance()->GetFBO("GBuffer");
		mDeferredMaterial->SetTexture("ENGINE_G_Buffer_Position", gBuffer->PositionBuffer, 0, false, true);
		mDeferredMaterial->SetTexture("ENGINE_G_Buffer_Normal", gBuffer->NormalBuffer, 1, false, true);
		mDeferredMaterial->SetTexture("ENGINE_G_Buffer_Albedo", gBuffer->ColorBuffer, 2, false, true);

		// 延迟渲染绘制
		renderAPI->SetRenderState(deferredRenderState);
		renderAPI->Draw(mScreenQuad->VAO);

		// 再接正向渲染绘制
		renderAPI->SetRenderState(opaqueRenderState);
		auto opaqueQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);
		opaqueQueue->Sort(camera, RenderSortType::FrontToBack);
		opaqueQueue->Batch();
		RenderBatches(opaqueQueue->GetBatches());

		renderAPI->SetRenderState(transparentRenderState);
		auto transparentQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Transparent);
		transparentQueue->Sort(camera, RenderSortType::BackToFront);
		transparentQueue->Batch();
		RenderBatches(transparentQueue->GetBatches());

		renderAPI->GenerateDrawCommand(mDrawCommandID);

		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}

	void RenderPassDeferredRendering::RenderBatches(const map<uint32_t, vector<MeshRenderer*>>& batchs)
	{
		auto engineProperties = RenderEngineProperties::GetInstance();

		for (auto& batch : batchs)
		{
			auto shader = batch.second[0]->mMatetrial->shader;
			shader->Use();

			for (auto renderer : batch.second)
			{
				auto material = renderer->mMatetrial;
				material->SetMaterialProperties();

				engineProperties->SetRendererProperties(renderer);

				material->SetEngineProperties();

				material->data->Use();

				renderer->Draw();
			}
		}
	}

	void RenderPassDeferredRendering::RenderSkyBox(Camera* camera)
	{
		// 先转3x3再回4x4，把相机位移信息去除
		Matrix4 mat_V = Matrix4(Matrix3(camera->GetViewMatrix()));
		Matrix4 mat_P = camera->GetProjectionMatrix();

		skyBoxMaterial->Use();
		skyBoxMaterial->SetMatrix("ENGINE_View", mat_V);
		skyBoxMaterial->SetMatrix("ENGINE_Projection", mat_P);
		skyBoxMaterial->SetCubeMap("_Skybox", SceneManager::GetInstance()->GetCurScene()->skyBox->GetID(), 0);

		RenderAPI::GetInstance()->Draw(skyBox->VAO);
	}
}