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
		// 这个是用在OpenGL的几何着色器里的，OpenGL是右手坐标系，所以这个得用基于右手坐标系的
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
		auto renderAPI = RenderAPI::GetInstance();
		// 切换到shadow FBO
		FBOManager::GetInstance()->SwitchFBO("ShadowCubeMap");
		// ViewPort改成渲染CubeMap的正方形
		renderAPI->SetViewPort(GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
		// 切换到阴影渲染设置
		renderAPI->SetRenderState(renderState);
		// 清理上一帧数据
		renderAPI->ClearFrameBuffer();

		// 基于左手坐标系构建6个方向上的VP矩阵
		Vector3 lightPos = light->GetTransform()->GetPosition();
		// 注意这里的shadowProj是基于右手坐标系构建的，因为OpenGL是基于右手坐标系的，这个是用在OpenGL的几何着色器里的
		// 如果是基于左手坐标系构建shadowProj，这里怎么设置都不对
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f)));
		shadowTransforms.push_back(shadowProj * Math::GetLookToMatrix(lightPos, Vector3( 0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)));

		// 渲染投射阴影的物体
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Qpaque);
		for (auto renderer : renderQueue->GetRenderers())
		{
			// 跳过不投射阴影的物体
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

		// 用完立刻清除，下一帧还会生成
		shadowTransforms.clear();

		renderAPI->GenerateDrawCommand(drawCommandID);
	}
}