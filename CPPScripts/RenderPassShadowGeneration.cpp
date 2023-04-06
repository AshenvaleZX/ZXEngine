#include "RenderPassShadowGeneration.h"
#include "RenderAPI.h"
#include "FBOManager.h"
#include "Light.h"
#include "Transform.h"
#include "ZShader.h"
#include "Resources.h"
#include "RenderQueueManager.h"
#include "MeshRenderer.h"
#include "GlobalData.h"
#include "RenderStateSetting.h"
#include "MaterialData.h"

namespace ZXEngine
{
	RenderPassShadowGeneration::RenderPassShadowGeneration()
	{
		// ���������OpenGL�ļ�����ɫ����ģ�OpenGL����������ϵ������������û�����������ϵ��
		shadowProj = Math::PerspectiveRH(Math::Deg2Rad(90.0f), (float)GlobalData::depthCubeMapWidth / (float)GlobalData::depthCubeMapWidth, GlobalData::shadowCubeMapNearPlane, GlobalData::shadowCubeMapFarPlane);
		shadowCubeMapShader = new Shader(Resources::GetAssetFullPath("Shaders/PointShadowDepth.zxshader", true), FrameBufferType::ShadowCubeMap);
		renderState = new RenderStateSetting();
		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ShadowGeneration);

		ClearInfo clearInfo = {};
		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT;
		FBOManager::GetInstance()->CreateFBO("ShadowCubeMap", FrameBufferType::ShadowCubeMap, clearInfo, GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
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
		// ע�������shadowProj�ǻ�����������ϵ�����ģ���ΪOpenGL�ǻ�����������ϵ�ģ����������OpenGL�ļ�����ɫ�����
		// ����ǻ�����������ϵ����shadowProj��������ô���ö�����
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)));

		// ��ȾͶ����Ӱ������
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Qpaque);
		for (auto renderer : renderQueue->GetRenderers())
		{
			// ������Ͷ����Ӱ������
			if (!renderer->castShadow)
				continue;

			if (renderer->shadowCastMaterial == nullptr)
				renderer->shadowCastMaterial = new Material(shadowCubeMapShader);

			renderer->shadowCastMaterial->Use();
			Matrix4 mat_M = renderer->GetTransform()->GetModelMatrix();
			renderer->shadowCastMaterial->SetMatrix("ENGINE_Model", mat_M);
			for (unsigned int i = 0; i < 6; ++i)
				renderer->shadowCastMaterial->SetMatrix("_ShadowMatrices", shadowTransforms[i], i);
			renderer->shadowCastMaterial->SetScalar("_FarPlane", GlobalData::shadowCubeMapFarPlane);
			renderer->shadowCastMaterial->SetVector("_LightPos", lightPos);

			renderer->Draw();
		}

		// ���������������һ֡��������
		shadowTransforms.clear();

		renderAPI->GenerateDrawCommand(drawCommandID);
	}
}