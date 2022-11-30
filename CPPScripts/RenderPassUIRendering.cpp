#include "RenderPassUIRendering.h"
#include "TextCharactersManager.h"
#include "UITextRenderer.h"
#include "RenderQueueManager.h"
#include "GameObject.h"

namespace ZXEngine
{
	RenderPassUIRendering::RenderPassUIRendering()
	{
		TextCharactersManager::Create();
	}

	void RenderPassUIRendering::Render(Camera* camera)
	{
		auto uiGameObjects = RenderQueueManager::GetInstance()->GetUIGameObjects();

		for (auto uiGameObject : uiGameObjects)
		{
			auto uiTextRenderer = uiGameObject->GetComponent<UITextRenderer>("UITextRenderer");
			uiTextRenderer->Render();
		}

		RenderQueueManager::GetInstance()->ClearUIGameObjects();
	}
}