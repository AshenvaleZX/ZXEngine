#include "RenderPassUIRendering.h"
#include "TextCharactersManager.h"
#include "Component/UITextRenderer.h"
#include "Component/UITextureRenderer.h"
#include "RenderQueueManager.h"
#include "GameObject.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	RenderPassUIRendering::RenderPassUIRendering()
	{
		UITextureRenderer::Init();
		TextCharactersManager::Create();

		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::UIRendering);
	}

	void RenderPassUIRendering::Render(Camera* camera)
	{
		auto view = camera->GetViewMatrix();
		auto projection = camera->GetProjectionMatrix();
		auto viewProjection = projection * view;
		auto uiGameObjects = RenderQueueManager::GetInstance()->GetUIGameObjects();

		for (auto uiGameObject : uiGameObjects)
		{
			// 绘制UI图片
			auto uiTextureRenderer = uiGameObject->GetComponent<UITextureRenderer>();
			if (uiTextureRenderer != nullptr && uiTextureRenderer->IsActive())
				uiTextureRenderer->Render(viewProjection);

			// 绘制UI文本
			auto uiTextRenderer = uiGameObject->GetComponent<UITextRenderer>();
			if (uiTextRenderer != nullptr && uiTextRenderer->IsActive())
				uiTextRenderer->Render(viewProjection);
		}

		RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);

		RenderQueueManager::GetInstance()->ClearUIGameObjects();
	}
}