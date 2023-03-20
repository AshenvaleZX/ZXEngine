#include "RenderPassForwardRendering.h"
#include "ZCamera.h"
#include "RenderQueueManager.h"
#include "ZShader.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Light.h"
#include "FBOManager.h"
#include "SceneManager.h"
#include "CubeMap.h"
#include "GlobalData.h"
#include "ParticleSystemManager.h"
#include "RenderStateSetting.h"
#include "RenderEngineProperties.h"

namespace ZXEngine
{
	RenderPassForwardRendering::RenderPassForwardRendering()
	{
		InitSkyBox();
		skyBoxShader = new Shader(Resources::GetAssetFullPath("Shaders/SkyBox.zxshader", true).c_str());

		skyBoxRenderState = new RenderStateSetting();
		skyBoxRenderState->depthTest = false;
		skyBoxRenderState->depthWrite = false;

		opaqueRenderState = new RenderStateSetting();

		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT | ZX_CLEAR_FRAME_BUFFER_STENCIL_BIT;

		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand();
	}

	void RenderPassForwardRendering::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		// 切换到主FBO
		FBOManager::GetInstance()->SwitchFBO("Main");
		// ViewPort设置为窗口大小
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);
		// 清理上一帧数据
		renderAPI->ClearFrameBuffer(clearInfo);
		
		// 渲染天空盒
		renderAPI->SetRenderState(skyBoxRenderState);
		RenderSkyBox(camera);

		// 渲染不透明队列
		renderAPI->SetRenderState(opaqueRenderState);
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Qpaque);
		renderQueue->Sort(camera);
		renderQueue->Batch();

		RenderEngineProperties::GetInstance()->SetCameraProperties(camera);
		RenderEngineProperties::GetInstance()->SetLightProperties(Light::GetAllLights()[0]);
		RenderEngineProperties::GetInstance()->SetShadowCubeMap(FBOManager::GetInstance()->GetFBO("ShadowCubeMap")->DepthBuffer);

		for (auto& batch : renderQueue->GetBatches())
		{
			auto shader = batch.second[0]->matetrial->shader;
			shader->Use();

			for (auto renderer : batch.second)
			{
				auto material = renderer->matetrial;
				shader->SetMaterialProperties(material);

				RenderEngineProperties::GetInstance()->SetRendererProperties(renderer);
				shader->SetEngineProperties();

				renderer->Draw();
			}
		}

		ParticleSystemManager::GetInstance()->Update();
		ParticleSystemManager::GetInstance()->Render(camera);

		renderAPI->GenerateDrawCommand(drawCommandID);

		// 每次渲染完要清空，下次要渲染的时候再重新添加
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
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
			vertex.Normal = Vector3(1);
			vertex.Tangent = Vector3(1);
			vertex.Bitangent = Vector3(1);
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

		skyBoxShader->Use();
		skyBoxShader->SetMat4("ENGINE_View", mat_V);
		skyBoxShader->SetMat4("ENGINE_Projection", mat_P);
		skyBoxShader->SetCubeMap("_Skybox", SceneManager::GetInstance()->GetCurScene()->skyBox->GetID(), 0);
		
		RenderAPI::GetInstance()->Draw(skyBox->VAO);
	}
}