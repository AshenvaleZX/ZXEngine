#include "RenderPassUIRendering.h"
#include "TextCharactersManager.h"
#include "UITextRenderer.h"
#include "UITextureRenderer.h"
#include "RenderQueueManager.h"
#include "GameObject.h"

namespace ZXEngine
{
	RenderPassUIRendering::RenderPassUIRendering()
	{
		UITextureRenderer::Init();
		TextCharactersManager::Create();
	}

	void RenderPassUIRendering::Render(Camera* camera)
	{
		auto uiGameObjects = RenderQueueManager::GetInstance()->GetUIGameObjects();

		for (auto uiGameObject : uiGameObjects)
		{
			auto uiTextRenderer = uiGameObject->GetComponent<UITextRenderer>("UITextRenderer");
			if (uiTextRenderer != nullptr)
				uiTextRenderer->Render();

			auto uiTextureRenderer = uiGameObject->GetComponent<UITextureRenderer>("UITextureRenderer");
			if (uiTextureRenderer != nullptr)
				uiTextureRenderer->Render();
		}

		RenderQueueManager::GetInstance()->ClearUIGameObjects();
	}
}