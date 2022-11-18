#include "RenderPassForwardRendering.h"
#include "ZCamera.h"
#include "RenderQueueManager.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Light.h"

namespace ZXEngine
{
	void RenderPassForwardRendering::Render(Camera* camera)
	{
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
}