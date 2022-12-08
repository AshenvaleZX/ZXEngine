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

namespace ZXEngine
{
	RenderPassShadowGeneration::RenderPassShadowGeneration()
	{
		// ���������OpenGL�ļ�����ɫ����ģ�OpenGL����������ϵ������������û�����������ϵ��
		shadowProj = Math::PerspectiveRH(Math::Deg2Rad(90.0f), (float)GlobalData::depthCubeMapWidth / (float)GlobalData::depthCubeMapWidth, GlobalData::shadowCubeMapNearPlane, GlobalData::shadowCubeMapFarPlane);
		shadowCubeMapShader = new Shader(Resources::GetAssetFullPath("Shaders/PointShadowDepth.zxshader").c_str());
		FBOManager::GetInstance()->CreateFBO("ShadowCubeMap", FrameBufferType::ShadowCubeMap, GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
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
		FBOManager::GetInstance()->SwitchFBO("ShadowCubeMap");
		// ViewPort�ĳ���ȾCubeMap��������
		RenderAPI::GetInstance()->SetViewPortSize(GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
		// ������Ȳ��Ժ�д��
		RenderAPI::GetInstance()->EnableDepthTest(true);
		RenderAPI::GetInstance()->EnableDepthWrite(true);
		// ������һ֡����
		RenderAPI::GetInstance()->ClearDepthBuffer();

		// ������������ϵ����6�������ϵ�VP����
		Vector3 lightPos = light->GetTransform()->position;
		// ע�������shadowProj�ǻ�����������ϵ�����ģ���ΪOpenGL�ǻ�����������ϵ�ģ����������OpenGL�ļ�����ɫ�����
		// ����ǻ�����������ϵ����shadowProj��������ô���ö�����
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)));

		// ����shader
		shadowCubeMapShader->Use();
		for (unsigned int i = 0; i < 6; ++i)
			shadowCubeMapShader->SetMat4("_ShadowMatrices[" + to_string(i) + "]", shadowTransforms[i]);
		shadowCubeMapShader->SetFloat("_FarPlane", GlobalData::shadowCubeMapFarPlane);
		shadowCubeMapShader->SetVec3("_LightPos", lightPos);

		// ���������������һ֡��������
		shadowTransforms.clear();

		// ��ȾͶ����Ӱ������
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue(RenderQueueType::Qpaque);
		for (auto renderer : renderQueue->GetRenderers())
		{
			// ������Ͷ����Ӱ������
			if (!renderer->castShadow)
				continue;

			Matrix4 mat_M = renderer->GetTransform()->GetModelMatrix();
			shadowCubeMapShader->SetMat4("_Model", mat_M);
			for (auto mesh : renderer->meshes)
			{
				mesh->Use();
				RenderAPI::GetInstance()->Draw();
			}
		}
	}
}