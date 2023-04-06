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

		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::UIRendering);
	}

	void RenderPassUIRendering::Render(Camera* camera)
	{
		auto uiGameObjects = RenderQueueManager::GetInstance()->GetUIGameObjects();

		for (auto uiGameObject : uiGameObjects)
		{
			// ����UIͼƬ
			auto uiTextureRenderer = uiGameObject->GetComponent<UITextureRenderer>();
			if (uiTextureRenderer != nullptr)
				uiTextureRenderer->Render();

			// ����UI�ı�
			auto uiTextRenderer = uiGameObject->GetComponent<UITextRenderer>();
			if (uiTextRenderer != nullptr)
				uiTextRenderer->Render();
		}

		RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);

		RenderQueueManager::GetInstance()->ClearUIGameObjects();
	}
}