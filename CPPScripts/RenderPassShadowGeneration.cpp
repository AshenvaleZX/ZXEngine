#include "RenderPassShadowGeneration.h"
#include "RenderAPI.h"
#include "FBOManager.h"
#include "Component/Light.h"
#include "Component/Transform.h"
#include "ZShader.h"
#include "Resources.h"
#include "RenderQueueManager.h"
#include "Component/MeshRenderer.h"
#include "GlobalData.h"
#include "RenderStateSetting.h"
#include "Material.h"
#include "MaterialData.h"
#include "RenderEngineProperties.h"
#include "ProjectSetting.h"

namespace ZXEngine
{
	RenderPassShadowGeneration::RenderPassShadowGeneration()
	{
#if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
		shadowProj = Math::PerspectiveRHNO(Math::Deg2Rad(90.0f), (float)GlobalData::depthCubeMapWidth / (float)GlobalData::depthCubeMapWidth, GlobalData::shadowCubeMapNearPlane, GlobalData::shadowCubeMapFarPlane);
#else
		shadowProj = Math::PerspectiveLHNO(Math::Deg2Rad(90.0f), (float)GlobalData::depthCubeMapWidth / (float)GlobalData::depthCubeMapWidth, GlobalData::shadowCubeMapNearPlane, GlobalData::shadowCubeMapFarPlane);
#endif
		shadowMapShader = new Shader(Resources::GetAssetFullPath("Shaders/DirectionalShadowDepth.zxshader", true), FrameBufferType::ShadowMap);
		animShadowMapShader = new Shader(Resources::GetAssetFullPath("Shaders/DirectionalShadowDepthAnim.zxshader", true), FrameBufferType::ShadowMap);

		if (ProjectSetting::isSupportGeometryShader)
		{
			shadowCubeMapShader = new Shader(Resources::GetAssetFullPath("Shaders/PointShadowDepth.zxshader", true), FrameBufferType::ShadowCubeMap);
			animShadowCubeMapShader = new Shader(Resources::GetAssetFullPath("Shaders/PointShadowDepthAnim.zxshader", true), FrameBufferType::ShadowCubeMap);
		}

		renderState = new RenderStateSetting();
		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ShadowGeneration);

		ClearInfo clearInfo = {};
		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT;
		FBOManager::GetInstance()->CreateFBO("ShadowMap", FrameBufferType::ShadowMap, clearInfo, GlobalData::depthMapWidth, GlobalData::depthMapWidth);
		FBOManager::GetInstance()->CreateFBO("ShadowCubeMap", FrameBufferType::ShadowCubeMap, clearInfo, GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
	}
	
	void RenderPassShadowGeneration::Render(Camera* camera)
	{
		if (Light::GetAllLights().empty())
			return;

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
		auto renderAPI = RenderAPI::GetInstance();
		// �л���shadow FBO
		FBOManager::GetInstance()->SwitchFBO("ShadowMap");
		// ViewPort�ĳ���ȾShadowMap��������
		renderAPI->SetViewPort(GlobalData::depthMapWidth, GlobalData::depthMapWidth);
		// �л�����Ӱ��Ⱦ����
		renderAPI->SetRenderState(renderState);
		// ������һ֡����
		renderAPI->ClearFrameBuffer();

		// ���������VP����
		Matrix4 shadowTransform = light->GetProjectionMatrix() * light->GetLightMatrix();
		RenderEngineProperties::GetInstance()->SetLightMatrix(shadowTransform);

		// ��ȾͶ����Ӱ������
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);
		for (auto renderer : renderQueue->GetRenderers())
		{
			// ������Ͷ����Ӱ������
			if (!renderer->mCastShadow)
				continue;

			if (renderer->mShadowCastMaterial == nullptr)
			{
				if (renderer->mAnimator)
					renderer->mShadowCastMaterial = new Material(animShadowMapShader);
				else
					renderer->mShadowCastMaterial = new Material(shadowMapShader);
			}

			Matrix4 mat_M = renderer->GetTransform()->GetModelMatrix();

			renderer->mShadowCastMaterial->Use();
			renderer->mShadowCastMaterial->SetMatrix("_ShadowMatrix", shadowTransform * mat_M);

			renderer->DrawShadow();
		}

		renderAPI->GenerateDrawCommand(drawCommandID);
	}

	void RenderPassShadowGeneration::RenderShadowCubeMap(Light* light)
	{
		if (!ProjectSetting::isSupportGeometryShader)
			return;

		auto renderAPI = RenderAPI::GetInstance();
		// �л���shadow FBO
		FBOManager::GetInstance()->SwitchFBO("ShadowCubeMap");
		// ViewPort�ĳ���ȾCubeMap��������
		renderAPI->SetViewPort(GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
		// �л�����Ӱ��Ⱦ����
		renderAPI->SetRenderState(renderState);
		// ������һ֡����
		renderAPI->ClearFrameBuffer();

		// ������������ϵ����6�������ϵ�VP����
		Vector3 lightPos = light->GetTransform()->GetPosition();
#if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)));
#else
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 1.0f,  0.0f,  0.0f), Vector3(0.0f,  1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f,  1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f,  1.0f), Vector3(0.0f,  1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f, -1.0f), Vector3(0.0f,  1.0f,  0.0f)));
#endif

		// ��ȾͶ����Ӱ������
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);
		for (auto renderer : renderQueue->GetRenderers())
		{
			// ������Ͷ����Ӱ������
			if (!renderer->mCastShadow)
				continue;

			if (renderer->mShadowCastMaterial == nullptr)
			{
				if (renderer->mAnimator)
					renderer->mShadowCastMaterial = new Material(animShadowCubeMapShader);
				else
					renderer->mShadowCastMaterial = new Material(shadowCubeMapShader);
			}

			renderer->mShadowCastMaterial->Use();
			Matrix4 mat_M = renderer->GetTransform()->GetModelMatrix();
			renderer->mShadowCastMaterial->SetMatrix("ENGINE_Model", mat_M);
			for (unsigned int i = 0; i < 6; ++i)
				renderer->mShadowCastMaterial->SetMatrix("_ShadowMatrices", shadowTransforms[i], i);
			renderer->mShadowCastMaterial->SetScalar("_FarPlane", GlobalData::shadowCubeMapFarPlane);
			renderer->mShadowCastMaterial->SetVector("_LightPos", lightPos);

			renderer->DrawShadow();
		}

		// ���������������һ֡��������
		shadowTransforms.clear();

		renderAPI->GenerateDrawCommand(drawCommandID);
	}
}