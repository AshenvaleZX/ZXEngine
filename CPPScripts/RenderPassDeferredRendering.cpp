#include "RenderPassDeferredRendering.h"
#include "RenderAPI.h"
#include "FBOManager.h"
#include "GlobalData.h"
#include "RenderQueueManager.h"
#include "RenderEngineProperties.h"
#include "Component/MeshRenderer.h"
#include "Material.h"
#include "MaterialData.h"
#include "ZShader.h"
#include "GeometryGenerator.h"
#include "Component/Light.h"
#include "Component/Transform.h"
#include "StaticMesh.h"

namespace ZXEngine
{
	RenderPassDeferredRendering::RenderPassDeferredRendering()
	{
		mDrawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::DeferredRendering);
		mScreenQuad = GeometryGenerator::CreateScreenQuad();

		mDeferredShader = new Shader(Resources::GetAssetFullPath("Shaders/DeferredRender.zxshader", true), FrameBufferType::Color);
		mDeferredMaterial = new Material(mDeferredShader);

		ClearInfo clearInfo = {};
		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT;
		FBOManager::GetInstance()->CreateFBO("Deferred", FrameBufferType::Color, clearInfo);
	}

	void RenderPassDeferredRendering::Render(Camera* camera)
	{
		auto renderAPI = RenderAPI::GetInstance();
		FBOManager::GetInstance()->SwitchFBO("Deferred");
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight);
		renderAPI->ClearFrameBuffer();

		mDeferredMaterial->Use();

		// Light
		const auto& allLights = Light::GetAllLights();
		mDeferredMaterial->SetScalar("_LightNum", static_cast<int32_t>(allLights.size()));

		vector<Vector4> lightPos;
		vector<Vector4> lightColor;
		for (auto light : allLights)
		{
			lightPos.push_back(light->GetTransform()->GetPosition());
			lightColor.push_back(light->color);
		}
		mDeferredMaterial->SetVector("_LightPositions", lightPos.data(), static_cast<uint32_t>(lightPos.size()));
		mDeferredMaterial->SetVector("_LightColors", lightColor.data(), static_cast<uint32_t>(lightColor.size()));

		// G-Buffer
		auto gBuffer = FBOManager::GetInstance()->GetFBO("GBuffer");
		mDeferredMaterial->SetTexture("ENGINE_G_Buffer_Position", gBuffer->PositionBuffer, 0, false, true);
		mDeferredMaterial->SetTexture("ENGINE_G_Buffer_Normal", gBuffer->NormalBuffer, 1, false, true);
		mDeferredMaterial->SetTexture("ENGINE_G_Buffer_Albedo", gBuffer->ColorBuffer, 2, false, true);

		renderAPI->Draw(mScreenQuad->VAO);

		renderAPI->GenerateDrawCommand(mDrawCommandID);

		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}
}