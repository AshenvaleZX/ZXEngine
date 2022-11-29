#include "RenderPassUIRendering.h"
#include "TextCharactersManager.h"
#include "TextRenderer.h"
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
			auto textRenderer = uiGameObject->GetComponent<TextRenderer>("TextRenderer");
			textRenderer->Render();
		}

		RenderQueueManager::GetInstance()->ClearUIGameObjects();
	}
}