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
		// 这个是用在OpenGL的几何着色器里的，OpenGL是右手坐标系，所以这个得用基于右手坐标系的
		shadowProj = Math::PerspectiveRH(Math::Deg2Rad(90.0f), (float)GlobalData::depthCubeMapWidth / (float)GlobalData::depthCubeMapWidth, GlobalData::shadowCubeMapNearPlane, GlobalData::shadowCubeMapFarPlane);
		shadowCubeMapShader = new Shader(Resources::GetAssetFullPath("Shaders/PointShadowDepth.zxshader").c_str());
		FBOManager::GetInstance()->CreateFBO("ShadowCubeMap", FrameBufferType::ShadowCubeMap, GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
	}
	
	void RenderPassShadowGeneration::Render(Camera* camera)
	{
		// 渲染阴影的光源
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
		// 切换到shadow FBO
		FBOManager::GetInstance()->SwitchFBO("ShadowCubeMap");
		// ViewPort改成渲染CubeMap的正方形
		RenderAPI::GetInstance()->SetViewPortSize(GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
		// 开启深度测试和写入
		RenderAPI::GetInstance()->EnableDepthTest(true);
		RenderAPI::GetInstance()->EnableDepthWrite(true);
		// 清理上一帧数据
		RenderAPI::GetInstance()->ClearDepthBuffer();

		// 基于左手坐标系构建6个方向上的VP矩阵
		Vector3 lightPos = light->GetTransform()->position;
		// 注意这里的shadowProj是基于右手坐标系构建的，因为OpenGL是基于右手坐标系的，这个是用在OpenGL的几何着色器里的
		// 如果是基于左手坐标系构建shadowProj，这里怎么设置都不对
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)));

		// 设置shader
		shadowCubeMapShader->Use();
		for (unsigned int i = 0; i < 6; ++i)
			shadowCubeMapShader->SetMat4("_ShadowMatrices[" + to_string(i) + "]", shadowTransforms[i]);
		shadowCubeMapShader->SetFloat("_FarPlane", GlobalData::shadowCubeMapFarPlane);
		shadowCubeMapShader->SetVec3("_LightPos", lightPos);

		// 用完立刻清除，下一帧还会生成
		shadowTransforms.clear();

		// 渲染投射阴影的物体
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue(RenderQueueType::Qpaque);
		for (auto renderer : renderQueue->GetRenderers())
		{
			// 跳过不投射阴影的物体
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