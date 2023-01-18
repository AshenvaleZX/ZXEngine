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
		// 切换到主FBO
		FBOManager::GetInstance()->SwitchFBO("Main");
		// ViewPort设置为窗口大小
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);
		// 清理上一帧数据
		renderAPI->ClearFrameBuffer();
		
		// 渲染天空盒
		renderAPI->SetRenderState(skyBoxRenderState);
		RenderSkyBox(camera);

		// 渲染不透明队列
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

				// 光源
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

				// 阴影
				if (renderer->receiveShadow)
				{
					Light* light = Light::GetAllLights()[0];
					if (light->type == LightType::Directional)
					{

					}
					else if (light->type == LightType::Point)
					{
						// 之前已经把纹理编号设置到 textureNum - 1 了，所以这里是textureNum
						shader->SetCubeMap("_DepthCubeMap", FBOManager::GetInstance()->GetFBO("ShadowCubeMap")->DepthBuffer, textureNum);
						shader->SetFloat("_FarPlane", GlobalData::shadowCubeMapFarPlane);
					}
				}

				renderer->Draw();
			}
		}

		ParticleSystemManager::GetInstance()->Update();
		ParticleSystemManager::GetInstance()->Render(camera);

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
		skyBoxShader->SetMat4("view", mat_V);
		skyBoxShader->SetMat4("projection", mat_P);
		skyBoxShader->SetCubeMap("skybox", SceneManager::GetInstance()->GetCurScene()->skyBox->GetID(), 0);
		
		skyBox->Use();

		RenderAPI::GetInstance()->Draw();
	}
}