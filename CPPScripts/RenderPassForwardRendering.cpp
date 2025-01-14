#include "RenderPassForwardRendering.h"
#include "Component/ZCamera.h"
#include "RenderQueueManager.h"
#include "ZShader.h"
#include "Material.h"
#include "Component/MeshRenderer.h"
#include "Component/Transform.h"
#include "Component/Light.h"
#include "FBOManager.h"
#include "SceneManager.h"
#include "CubeMap.h"
#include "GlobalData.h"
#include "ParticleSystem/ParticleSystemManager.h"
#include "RenderStateSetting.h"
#include "RenderEngineProperties.h"
#include "MaterialData.h"
#include "StaticMesh.h"
#include "GeometryGenerator.h"

namespace ZXEngine
{
	RenderPassForwardRendering::RenderPassForwardRendering()
	{
		skyBox = GeometryGenerator::CreateSimpleInwardBox();
		skyBoxMaterial = new Material(new Shader(Resources::GetAssetFullPath("Shaders/SkyBox.zxshader", true), FrameBufferType::Normal));

		skyBoxRenderState = new RenderStateSetting();
		skyBoxRenderState->depthTest = false;
		skyBoxRenderState->depthWrite = false;

		opaqueRenderState = new RenderStateSetting();

		transparentRenderState = new RenderStateSetting();
		transparentRenderState->depthWrite = false;

		blitCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::NotCare, ZX_CLEAR_FRAME_BUFFER_NONE_BIT);
		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ForwardRendering, ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);

		ClearInfo clearInfo = {};
		FBOManager::GetInstance()->CreateFBO("Forward", FrameBufferType::Normal, clearInfo);

		FBOManager::GetInstance()->CreateFBO("ForwardDepth", FrameBufferType::ShadowMap);

		RenderEngineProperties::GetInstance()->SetDepthMap(FBOManager::GetInstance()->GetFBO("ForwardDepth")->DepthBuffer);
	}

	void RenderPassForwardRendering::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		// 切换到主FBO
		FBOManager::GetInstance()->SwitchFBO("Forward");
		// ViewPort设置为窗口大小
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);
		// 清理上一帧数据
		renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);
		
		// 渲染天空盒
		renderAPI->SetRenderState(skyBoxRenderState);
		RenderSkyBox(camera);

		// 设置引擎参数
		RenderEngineProperties::GetInstance()->SetCameraProperties(camera);
		RenderEngineProperties::GetInstance()->SetLightProperties(Light::GetAllLights());

		// 渲染不透明队列
		renderAPI->SetRenderState(opaqueRenderState);
		auto opaqueQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);
		opaqueQueue->Sort(camera, RenderSortType::FrontToBack);
		opaqueQueue->Batch();
		RenderBatches(opaqueQueue->GetBatches());

		// 渲染半透明队列
		renderAPI->SetRenderState(transparentRenderState);
		auto transparentQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Transparent);
		transparentQueue->Sort(camera, RenderSortType::BackToFront);
		transparentQueue->Batch();
		RenderBatches(transparentQueue->GetBatches());

		// 渲染粒子系统
		ParticleSystemManager::GetInstance()->Render(camera);

		renderAPI->GenerateDrawCommand(drawCommandID);

		renderAPI->BlitFrameBuffer(blitCommandID, "Forward", "ForwardDepth", ZX_FRAME_BUFFER_PIECE_DEPTH);

		// 每次渲染完要清空，下次要渲染的时候再重新添加
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}

	void RenderPassForwardRendering::RenderBatches(const map<uint32_t, vector<MeshRenderer*>>& batchs)
	{
		auto engineProperties = RenderEngineProperties::GetInstance();
		auto shadowMapID = FBOManager::GetInstance()->GetFBO("ShadowMap")->DepthBuffer;
		auto shadowCubeMapID = FBOManager::GetInstance()->GetFBO("ShadowCubeMap")->DepthBuffer;

		for (auto& batch : batchs)
		{
			auto shader = batch.second[0]->mMatetrial->shader;
			shader->Use();

			for (auto renderer : batch.second)
			{
				auto material = renderer->mMatetrial;
				material->SetMaterialProperties();

				engineProperties->SetRendererProperties(renderer);

				if (renderer->mReceiveShadow)
				{
					engineProperties->SetShadowMap(shadowMapID);
					engineProperties->SetShadowCubeMap(shadowCubeMapID);
				}
				else
				{
					engineProperties->SetEmptyShadowMap();
					engineProperties->SetEmptyShadowCubeMap();
				}

				material->SetEngineProperties();

				material->data->Use();

				renderer->Draw();
			}
		}
	}

	void RenderPassForwardRendering::RenderSkyBox(Camera* camera)
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