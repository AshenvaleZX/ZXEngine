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

namespace ZXEngine
{
	RenderPassShadowGeneration::RenderPassShadowGeneration()
	{
#if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
		shadowProj = Math::PerspectiveRH(Math::Deg2Rad(90.0f), (float)GlobalData::depthCubeMapWidth / (float)GlobalData::depthCubeMapWidth, GlobalData::shadowCubeMapNearPlane, GlobalData::shadowCubeMapFarPlane);
#else
		shadowProj = Math::PerspectiveLH(Math::Deg2Rad(90.0f), (float)GlobalData::depthCubeMapWidth / (float)GlobalData::depthCubeMapWidth, GlobalData::shadowCubeMapNearPlane, GlobalData::shadowCubeMapFarPlane);
#endif
		shadowCubeMapShader = new Shader(Resources::GetAssetFullPath("Shaders/PointShadowDepth.zxshader", true), FrameBufferType::ShadowCubeMap);
		renderState = new RenderStateSetting();
		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ShadowGeneration);

		ClearInfo clearInfo = {};
		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT;
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

	}

	void RenderPassShadowGeneration::RenderShadowCubeMap(Light* light)
	{
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
				renderer->mShadowCastMaterial = new Material(shadowCubeMapShader);

			renderer->mShadowCastMaterial->Use();
			Matrix4 mat_M = renderer->GetTransform()->GetModelMatrix();
			renderer->mShadowCastMaterial->SetMatrix("ENGINE_Model", mat_M);
			for (unsigned int i = 0; i < 6; ++i)
				renderer->mShadowCastMaterial->SetMatrix("_ShadowMatrices", shadowTransforms[i], i);
			renderer->mShadowCastMaterial->SetScalar("_FarPlane", GlobalData::shadowCubeMapFarPlane);
			renderer->mShadowCastMaterial->SetVector("_LightPos", lightPos);

			renderer->Draw();
		}

		// ���������������һ֡��������
		shadowTransforms.clear();

		renderAPI->GenerateDrawCommand(drawCommandID);
	}
}