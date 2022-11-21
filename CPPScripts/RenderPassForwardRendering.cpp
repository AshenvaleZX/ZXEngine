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

namespace ZXEngine
{
	RenderPassForwardRendering::RenderPassForwardRendering()
	{
		InitSkyBox();
		skyBoxShader = new Shader(Resources::GetAssetFullPath("Shaders/SkyBox.zxshader").c_str());
	}

	void RenderPassForwardRendering::Render(Camera* camera)
	{
		// 切换到主FBO
		RenderAPI::GetInstance()->SwitchFrameBuffer(FBOManager::GetInstance()->mainFBO->ID);
		// 开启深度测试
		RenderAPI::GetInstance()->EnableDepthTest(true);
		// 妈的，深度写入的状态设置居然是跨FBO的，在渲染其它FBO时的设置也会影响这里，所以在Clear深度缓冲之前，为了确保没问题先开启一下深度写入，因为Clear深度缓冲需要在开启深度写入状态下执行
		RenderAPI::GetInstance()->EnableDepthWrite(true);
		// 清理上一帧数据
		RenderAPI::GetInstance()->ClearFrameBuffer();

		// 关闭深度写入，渲染天空盒
		RenderAPI::GetInstance()->EnableDepthWrite(false);
		RenderSkyBox(camera);
		// 重新打开深度写入
		RenderAPI::GetInstance()->EnableDepthWrite(true);

		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue(RenderQueueType::Qpaque);

		mat4 mat_V = camera->GetViewMatrix();
		mat4 mat_P = camera->GetProjectionMatrix();
		for (auto renderer : renderQueue->GetRenderers())
		{
			Material* material = renderer->matetrial;
			Shader* shader = material->shader;
			mat4 mat_M = renderer->GetTransform()->GetModelMatrix();
			shader->Use();
			shader->SetMat4("model", mat_M);
			shader->SetMat4("view", mat_V);
			shader->SetMat4("projection", mat_P);

			for (unsigned int i = 0; i < material->textures.size(); i++)
			{
				shader->SetTexture(material->textures[i].first, material->textures[i].second->GetID(), i);
			}

			if (shader->GetLightType() == LightType::Directional)
			{
				Light* light = Light::GetAllLights()[0];
				shader->SetVec3("viewPos", camera->GetTransform()->position);
				shader->SetVec3("dirLight.direction", light->GetTransform()->GetForward());
				shader->SetVec3("dirLight.color", light->color);
				shader->SetFloat("dirLight.intensity", light->intensity);
			}
			else if (shader->GetLightType() == LightType::Point)
			{
				Light* light = Light::GetAllLights()[0];
				shader->SetVec3("viewPos", camera->GetTransform()->position);
				shader->SetVec3("pointLight.position", light->GetTransform()->position);
				shader->SetVec3("pointLight.color", light->color);
				shader->SetFloat("pointLight.intensity", light->intensity);
			}

			for (auto mesh : renderer->meshes)
			{
				mesh->Use();

				RenderAPI::GetInstance()->Draw();
			}
		}

		// 每次渲染完要清空，下次要渲染的时候再重新添加
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}

	void RenderPassForwardRendering::InitSkyBox()
	{
		// 这里自己在代码里写一个Box模型，就不从硬盘加载了
		vec3 points[8] =
		{
			vec3(1, 1, 1),
			vec3(1, 1, -1),
			vec3(1, -1, 1),
			vec3(1, -1, -1),
			vec3(-1, 1, 1),
			vec3(-1, 1, -1),
			vec3(-1, -1, 1),
			vec3(-1, -1, -1)
		};
		vector<Vertex> vertices;
		vector<unsigned int> indices =
		{
			// 前
			1,5,7,
			1,7,3,
			// 右
			0,1,3,
			0,3,2,
			// 后
			6,4,0,
			6,0,2,
			// 左
			4,7,5,
			4,6,7,
			// 上
			0,5,1,
			0,4,5,
			// 下
			2,3,7,
			7,6,2
		};
		for (unsigned int i = 0; i < 8; i++)
		{
			Vertex vertex;
			vertex.Position = points[i];
			vertex.Normal = vec3(1);
			vertex.Tangent = vec3(1);
			vertex.Bitangent = vec3(1);
			vertex.TexCoords = vec2(1);
			vertices.push_back(vertex);
		}
		skyBox = new Mesh(vertices, indices);
	}

	void RenderPassForwardRendering::RenderSkyBox(Camera* camera)
	{
		// 先转3x3再回4x4，把相机位移信息去除
		mat4 mat_V = mat4(mat3(camera->GetViewMatrix()));
		mat4 mat_P = camera->GetProjectionMatrix();

		skyBoxShader->Use();
		skyBoxShader->SetMat4("view", mat_V);
		skyBoxShader->SetMat4("projection", mat_P);
		skyBoxShader->SetCubeMap("skybox", SceneManager::GetInstance()->GetCurScene()->skyBox->GetID(), 0);

		skyBox->Use();

		RenderAPI::GetInstance()->Draw();
	}
}