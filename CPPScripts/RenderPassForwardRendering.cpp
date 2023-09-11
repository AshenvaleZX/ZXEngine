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
#include "ParticleSystemManager.h"
#include "RenderStateSetting.h"
#include "RenderEngineProperties.h"
#include "MaterialData.h"
#include "StaticMesh.h"

namespace ZXEngine
{
	RenderPassForwardRendering::RenderPassForwardRendering()
	{
		InitSkyBox();
		skyBoxMaterial = new Material(new Shader(Resources::GetAssetFullPath("Shaders/SkyBox.zxshader", true), FrameBufferType::Normal));

		skyBoxRenderState = new RenderStateSetting();
		skyBoxRenderState->depthTest = false;
		skyBoxRenderState->depthWrite = false;

		opaqueRenderState = new RenderStateSetting();

		transparentRenderState = new RenderStateSetting();
		transparentRenderState->depthWrite = false;

		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ForwardRendering);

		ClearInfo clearInfo = {};
		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT;
		FBOManager::GetInstance()->CreateFBO("Forward", FrameBufferType::Normal, clearInfo);
	}

	void RenderPassForwardRendering::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		// 切换到主FBO
		FBOManager::GetInstance()->SwitchFBO("Forward");
		// ViewPort设置为窗口大小
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);
		// 清理上一帧数据
		renderAPI->ClearFrameBuffer();
		
		// 渲染天空盒
		renderAPI->SetRenderState(skyBoxRenderState);
		RenderSkyBox(camera);

		// 设置引擎参数
		RenderEngineProperties::GetInstance()->SetCameraProperties(camera);
		RenderEngineProperties::GetInstance()->SetLightProperties(Light::GetAllLights());
		RenderEngineProperties::GetInstance()->SetShadowCubeMap(FBOManager::GetInstance()->GetFBO("ShadowCubeMap")->DepthBuffer);

		// 渲染不透明队列
		renderAPI->SetRenderState(opaqueRenderState);
		auto opaqueQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);
		opaqueQueue->Sort(camera);
		opaqueQueue->Batch();
		RenderBatches(opaqueQueue->GetBatches());

		// 渲染半透明队列
		renderAPI->SetRenderState(transparentRenderState);
		auto transparentQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Transparent);
		transparentQueue->Sort(camera);
		transparentQueue->Batch();
		RenderBatches(transparentQueue->GetBatches());

		// 渲染粒子系统
		ParticleSystemManager::GetInstance()->Render(camera);

		renderAPI->GenerateDrawCommand(drawCommandID);

		// 每次渲染完要清空，下次要渲染的时候再重新添加
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}

	void RenderPassForwardRendering::RenderBatches(const map<uint32_t, vector<MeshRenderer*>>& batchs)
	{
		for (auto& batch : batchs)
		{
			auto shader = batch.second[0]->mMatetrial->shader;
			shader->Use();

			for (auto renderer : batch.second)
			{
				auto material = renderer->mMatetrial;
				material->SetMaterialProperties();

				RenderEngineProperties::GetInstance()->SetRendererProperties(renderer);
				material->SetEngineProperties();

				renderer->UpdateAnimation();

				material->data->Use();

				renderer->UpdateBoneTransformsForRender();

				renderer->Draw();
			}
		}
	}

	void RenderPassForwardRendering::InitSkyBox()
	{
		// 这里自己在代码里写一个Box模型，就不从硬盘加载了
		Vector3 points[8] =
		{
			Vector3(1, 1, 1),
			Vector3(1, 1, -1),
			Vector3(1, -1, 1),
			Vector3(1, -1, -1),
			Vector3(-1, 1, 1),
			Vector3(-1, 1, -1),
			Vector3(-1, -1, 1),
			Vector3(-1, -1, -1)
		};
		vector<Vertex> vertices;
		vector<unsigned int> indices =
		{
			// 前
			7,5,1,
			3,7,1,
			// 右
			3,1,0,
			2,3,0,
			// 后
			0,4,6,
			2,0,6,
			// 左
			5,7,4,
			7,6,4,
			// 上
			1,5,0,
			5,4,0,
			// 下
			7,3,2,
			2,6,7
		};
		for (unsigned int i = 0; i < 8; i++)
		{
			Vertex vertex;
			vertex.Position = points[i];
			vertex.TexCoords = Vector2(1);
			vertices.push_back(vertex);
		}
		skyBox = new StaticMesh(vertices, indices);
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