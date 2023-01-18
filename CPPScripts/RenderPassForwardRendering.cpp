#include "RenderPassForwardRendering.h"
#include "ZCamera.h"
#include "RenderQueueManager.h"
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
	}

	void RenderPassForwardRendering::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		// �л�����FBO
		FBOManager::GetInstance()->SwitchFBO("Main");
		// ViewPort����Ϊ���ڴ�С
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);
		// ������һ֡����
		renderAPI->ClearFrameBuffer();
		
		// ��Ⱦ��պ�
		renderAPI->SetRenderState(skyBoxRenderState);
		RenderSkyBox(camera);

		// ��Ⱦ��͸������
		renderAPI->SetRenderState(opaqueRenderState);
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Qpaque);
		renderQueue->Sort(camera);
		renderQueue->Batch();

		auto mat_V = camera->GetViewMatrix();
		auto mat_P = camera->GetProjectionMatrix();

		for (auto& batch : renderQueue->GetBatches())
		{
			auto shader = batch.second[0]->matetrial->shader;
			shader->Use();
			shader->SetMat4("view", mat_V);
			shader->SetMat4("projection", mat_P);

			for (auto renderer : batch.second)
			{
				auto material = renderer->matetrial;

				auto mat_M = renderer->GetTransform()->GetModelMatrix();
				shader->SetMat4("model", mat_M);

				unsigned int textureNum = (unsigned int)material->textures.size();
				for (unsigned int i = 0; i < textureNum; i++)
				{
					shader->SetTexture(material->textures[i].first, material->textures[i].second->GetID(), i);
				}

				// ��Դ
				if (shader->GetLightType() == LightType::Directional)
				{
					Light* light = Light::GetAllLights()[0];
					shader->SetVec3("viewPos", camera->GetTransform()->GetPosition());
					shader->SetVec3("dirLight.direction", light->GetTransform()->GetForward());
					shader->SetVec3("dirLight.color", light->color);
					shader->SetFloat("dirLight.intensity", light->intensity);
				}
				else if (shader->GetLightType() == LightType::Point)
				{
					Light* light = Light::GetAllLights()[0];
					shader->SetVec3("viewPos", camera->GetTransform()->GetPosition());
					shader->SetVec3("pointLight.position", light->GetTransform()->GetPosition());
					shader->SetVec3("pointLight.color", light->color);
					shader->SetFloat("pointLight.intensity", light->intensity);
				}

				// ��Ӱ
				if (renderer->receiveShadow)
				{
					Light* light = Light::GetAllLights()[0];
					if (light->type == LightType::Directional)
					{

					}
					else if (light->type == LightType::Point)
					{
						// ֮ǰ�Ѿ������������õ� textureNum - 1 �ˣ�����������textureNum
						shader->SetCubeMap("_DepthCubeMap", FBOManager::GetInstance()->GetFBO("ShadowCubeMap")->DepthBuffer, textureNum);
						shader->SetFloat("_FarPlane", GlobalData::shadowCubeMapFarPlane);
					}
				}

				renderer->Draw();
			}
		}

		ParticleSystemManager::GetInstance()->Update();
		ParticleSystemManager::GetInstance()->Render(camera);

		// ÿ����Ⱦ��Ҫ��գ��´�Ҫ��Ⱦ��ʱ�����������
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
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
		// ��ת3x3�ٻ�4x4�������λ����Ϣȥ��
		Matrix4 mat_V = Matrix4(Matrix3(camera->GetViewMatrix()));
		Matrix4 mat_P = camera->GetProjectionMatrix();

		skyBoxShader->Use();
		skyBoxShader->SetMat4("view", mat_V);
		skyBoxShader->SetMat4("projection", mat_P);
		skyBoxShader->SetCubeMap("skybox", SceneManager::GetInstance()->GetCurScene()->skyBox->GetID(), 0);
		
		skyBox->Use();

		RenderAPI::GetInstance()->Draw();
	}
}