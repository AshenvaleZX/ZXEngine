#include "RenderPassShadowGeneration.h"
#include "RenderAPI.h"
#include "FBOManager.h"
#include "Light.h"
#include "Transform.h"
#include "ZShader.h"
#include "Resources.h"
#include "RenderQueueManager.h"
#include "MeshRenderer.h"

namespace ZXEngine
{
	RenderPassShadowGeneration::RenderPassShadowGeneration()
	{
		shadowProj = perspective(radians(90.0f), (float)DEPTH_CUBEMAP_WIDTH / (float)DEPTH_CUBEMAP_WIDTH, nearPlane, farPlane);
		shadowCubeMapShader = new Shader(Resources::GetAssetFullPath("Shaders/PointShadowDepth.zxshader").c_str());
	}
	
	void RenderPassShadowGeneration::Render(Camera* camera)
	{
		// ��Ⱦ��Ӱ�Ĺ�Դ
		Light* light = Light::GetAllLights()[0];

		if (light->type == LightType::Directional)
		{
			RenderShadowMap(light);
		}
		else if (light->type == LightType::Point)
		{
			RenderShadowCubeMap(light);
		}
	}

	void RenderPassShadowGeneration::RenderShadowMap(Light* light)
	{

	}

	void RenderPassShadowGeneration::RenderShadowCubeMap(Light* light)
	{
		// �л���shadow FBO
		RenderAPI::GetInstance()->SwitchFrameBuffer(FBOManager::GetInstance()->shadowCubeMapFBO->ID);

		// ����6�������ϵ�VP����
		vec3 lightPos = light->GetTransform()->position;
		shadowTransforms.push_back(shadowProj * Utils::GetLookToMatrix(lightPos, vec3( 1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Utils::GetLookToMatrix(lightPos, vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Utils::GetLookToMatrix(lightPos, vec3( 0.0f,  1.0f,  0.0f), vec3(0.0f,  0.0f,  1.0f)));
		shadowTransforms.push_back(shadowProj * Utils::GetLookToMatrix(lightPos, vec3( 0.0f, -1.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * Utils::GetLookToMatrix(lightPos, vec3( 0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Utils::GetLookToMatrix(lightPos, vec3( 0.0f,  0.0f, -1.0f), vec3(0.0f, -1.0f,  0.0f)));

		// ViewPort�ĳ���ȾCubeMap��������
		RenderAPI::GetInstance()->SetViewPortSize(DEPTH_CUBEMAP_WIDTH, DEPTH_CUBEMAP_WIDTH);
		// ������һ֡����
		RenderAPI::GetInstance()->ClearDepthBuffer();

		// ����shader
		shadowCubeMapShader->Use();
		for (unsigned int i = 0; i < 6; ++i)
			shadowCubeMapShader->SetMat4("_ShadowMatrices[" + to_string(i) + "]", shadowTransforms[i]);
		shadowCubeMapShader->SetFloat("_FarPlane", farPlane);
		shadowCubeMapShader->SetVec3("_LightPos", lightPos);

		// ��ȾͶ����Ӱ������
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue(RenderQueueType::Qpaque);
		for (auto renderer : renderQueue->GetRenderers())
		{
			// ������Ͷ����Ӱ������
			if (!renderer->castShadow)
				continue;

			mat4 mat_M = renderer->GetTransform()->GetModelMatrix();
			shadowCubeMapShader->SetMat4("_Model", mat_M);
			for (auto mesh : renderer->meshes)
			{
				mesh->Use();
				RenderAPI::GetInstance()->Draw();
			}
		}
	}
}