#include "RenderPassUIRendering.h"
#include "TextCharactersManager.h"
#include "TextRenderer.h"

namespace ZXEngine
{
	RenderPassUIRendering::RenderPassUIRendering()
	{
		TextCharactersManager::Create();
		text = new TextRenderer();
		text->text = "ZX Test";
		text->pos = vec2(20, 680);
		text->scale = 0.5;
	}

	void RenderPassUIRendering::Render(Camera* camera)
	{
		text->Render();
	}
}