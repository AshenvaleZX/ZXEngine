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
		else
		{
			shadowCubeMapShaderNonGS = new Shader(Resources::GetAssetFullPath("Shaders/NonGSPointShadowDepth.zxshader", true), FrameBufferType::ShadowCubeMap);
			animShadowCubeMapShaderNonGS = new Shader(Resources::GetAssetFullPath("Shaders/NonGSPointShadowDepthAnim.zxshader", true), FrameBufferType::ShadowCubeMap);
		}

		renderState = new RenderStateSetting();
		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ShadowGeneration, ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);
		for (uint32_t i = 0; i < 6; i++)
		{
			drawCommandIDs.push_back(RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ShadowGeneration, ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT));
		}

		ClearInfo clearInfo = {};
		FBOManager::GetInstance()->CreateFBO("ShadowMap", FrameBufferType::ShadowMap, clearInfo, GlobalData::depthMapWidth, GlobalData::depthMapWidth);
		FBOManager::GetInstance()->CreateFBO("ShadowCubeMap", FrameBufferType::ShadowCubeMap, clearInfo, GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
	}
	
	void RenderPassShadowGeneration::Render(Camera* camera)
	{
		if (Light::GetAllLights().empty())
			return;

		// 渲染阴影的光源
		Light* light = Light::GetAllLights()[0];

		if (light->mType == LightType::Directional)
		{
			RenderShadowMap(light);
		}
		else if (light->mType == LightType::Point)
		{
			RenderShadowCubeMap(light);
		}
	}

	void RenderPassShadowGeneration::RenderShadowMap(Light* light)
	{
		auto renderAPI = RenderAPI::GetInstance();
		// 切换到shadow FBO
		FBOManager::GetInstance()->SwitchFBO("ShadowMap");
		// ViewPort改成渲染ShadowMap的正方形
		renderAPI->SetViewPort(GlobalData::depthMapWidth, GlobalData::depthMapWidth);
		// 切换到阴影渲染设置
		renderAPI->SetRenderState(renderState);
		// 清理上一帧数据
		renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);

		// 类似相机的VP矩阵
		Matrix4 shadowTransform = light->GetProjectionMatrix() * light->GetLightMatrix();
		RenderEngineProperties::GetInstance()->SetLightMatrix(shadowTransform);

		// 渲染投射阴影的物体
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);
		for (auto renderer : renderQueue->GetRenderers())
		{
			// 跳过不投射阴影的物体
			if (!renderer->mCastShadow)
				continue;

			if (renderer->mShadowCastMaterial == nullptr)
			{
#ifdef ZX_COMPUTE_ANIMATION
				renderer->mShadowCastMaterial = new Material(shadowMapShader);
#else
				if (renderer->mAnimator)
					renderer->mShadowCastMaterial = new Material(animShadowMapShader);
				else
					renderer->mShadowCastMaterial = new Material(shadowMapShader);
#endif
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
		// 基于左手坐标系构建6个方向上的VP矩阵
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

		if (ProjectSetting::isSupportGeometryShader)
			RenderShadowCubeMapWithGS(lightPos);
		else
			RenderShadowCubeMapWithoutGS(lightPos);

		// 用完立刻清除，下一帧还会生成
		shadowTransforms.clear();
	}

	void RenderPassShadowGeneration::RenderShadowCubeMapWithGS(const Vector3& lightPos)
	{
		auto renderAPI = RenderAPI::GetInstance();
		// 切换到shadow FBO
		FBOManager::GetInstance()->SwitchFBO("ShadowCubeMap");
		// ViewPort改成渲染CubeMap的正方形
		renderAPI->SetViewPort(GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
		// 切换到阴影渲染设置
		renderAPI->SetRenderState(renderState);
		// 清理上一帧数据
		renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);

		// 渲染投射阴影的物体
		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);
		for (auto renderer : renderQueue->GetRenderers())
		{
			// 跳过不投射阴影的物体
			if (!renderer->mCastShadow)
				continue;

			if (renderer->mShadowCastMaterial == nullptr)
			{
#ifdef ZX_COMPUTE_ANIMATION
				renderer->mShadowCastMaterial = new Material(shadowCubeMapShader);
#else
				if (renderer->mAnimator)
					renderer->mShadowCastMaterial = new Material(animShadowCubeMapShader);
				else
					renderer->mShadowCastMaterial = new Material(shadowCubeMapShader);
#endif
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

		renderAPI->GenerateDrawCommand(drawCommandID);
	}

	void RenderPassShadowGeneration::RenderShadowCubeMapWithoutGS(const Vector3& lightPos)
	{
		auto renderAPI = RenderAPI::GetInstance();
		renderAPI->SetViewPort(GlobalData::depthCubeMapWidth, GlobalData::depthCubeMapWidth);
		renderAPI->SetRenderState(renderState);

		for (uint32_t i = 0; i < 6; i++)
		{
			FBOManager::GetInstance()->SwitchFBO("ShadowCubeMap", i);
			renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);

			// 渲染投射阴影的物体
			auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue((int)RenderQueueType::Opaque);
			for (auto renderer : renderQueue->GetRenderers())
			{
				// 跳过不投射阴影的物体
				if (!renderer->mCastShadow)
					continue;

				if (renderer->mNonGSCubeShadowCastMaterials.size() == 0)
				{
					for (uint32_t j = 0; j < 6; j++)
					{
#ifdef ZX_COMPUTE_ANIMATION
						renderer->mNonGSCubeShadowCastMaterials.push_back(new Material(shadowCubeMapShaderNonGS));
#else
						if (renderer->mAnimator)
							renderer->mNonGSCubeShadowCastMaterials.push_back(new Material(animShadowCubeMapShaderNonGS));
						else
							renderer->mNonGSCubeShadowCastMaterials.push_back(new Material(shadowCubeMapShaderNonGS));
#endif
					}
				}

				Matrix4 mat_M = renderer->GetTransform()->GetModelMatrix();

				renderer->mNonGSCubeShadowCastMaterials[i]->Use();
				renderer->mNonGSCubeShadowCastMaterials[i]->SetMatrix("ENGINE_Model", mat_M);
				renderer->mNonGSCubeShadowCastMaterials[i]->SetMatrix("_ShadowMatrix", shadowTransforms[i] * mat_M);
				renderer->mNonGSCubeShadowCastMaterials[i]->SetScalar("_FarPlane", GlobalData::shadowCubeMapFarPlane);
				renderer->mNonGSCubeShadowCastMaterials[i]->SetVector("_LightPos", lightPos);

				renderer->DrawShadow(i);
			}

			renderAPI->GenerateDrawCommand(drawCommandIDs[i]);
		}
	}
}