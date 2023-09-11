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
		// �л�����FBO
		FBOManager::GetInstance()->SwitchFBO("Forward");
		// ViewPort����Ϊ���ڴ�С
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);
		// ������һ֡����
		renderAPI->ClearFrameBuffer();
		
		// ��Ⱦ��պ�
		renderAPI->SetRenderState(skyBoxRenderState);
		RenderSkyBox(camera);

		// �����������
		RenderEngineProperties::GetInstance()->SetCameraProperties(camera);
		RenderEngineProperties::GetInstance()->SetLightProperties(Light::GetAllLights());
		RenderEngineProperties::GetInstance()->SetShadowCubeMap(FBOManager::GetInstance()->GetFBO("ShadowCubeMap")->DepthBuffer);

		// ��Ⱦ��͸������
		renderAPI->SetRenderState(opaqueRenderState);
		auto opaqueQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);
		opaqueQueue->Sort(camera);
		opaqueQueue->Batch();
		RenderBatches(opaqueQueue->GetBatches());

		// ��Ⱦ��͸������
		renderAPI->SetRenderState(transparentRenderState);
		auto transparentQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Transparent);
		transparentQueue->Sort(camera);
		transparentQueue->Batch();
		RenderBatches(transparentQueue->GetBatches());

		// ��Ⱦ����ϵͳ
		ParticleSystemManager::GetInstance()->Render(camera);

		renderAPI->GenerateDrawCommand(drawCommandID);

		// ÿ����Ⱦ��Ҫ��գ��´�Ҫ��Ⱦ��ʱ�����������
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
		// �����Լ��ڴ�����дһ��Boxģ�ͣ��Ͳ���Ӳ�̼�����
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
			// ǰ
			7,5,1,
			3,7,1,
			// ��
			3,1,0,
			2,3,0,
			// ��
			0,4,6,
			2,0,6,
			// ��
			5,7,4,
			7,6,4,
			// ��
			1,5,0,
			5,4,0,
			// ��
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
		// ��ת3x3�ٻ�4x4�������λ����Ϣȥ��
		Matrix4 mat_V = Matrix4(Matrix3(camera->GetViewMatrix()));
		Matrix4 mat_P = camera->GetProjectionMatrix();

		skyBoxMaterial->Use();
		skyBoxMaterial->SetMatrix("ENGINE_View", mat_V);
		skyBoxMaterial->SetMatrix("ENGINE_Projection", mat_P);
		skyBoxMaterial->SetCubeMap("_Skybox", SceneManager::GetInstance()->GetCurScene()->skyBox->GetID(), 0);
		
		RenderAPI::GetInstance()->Draw(skyBox->VAO);
	}
}