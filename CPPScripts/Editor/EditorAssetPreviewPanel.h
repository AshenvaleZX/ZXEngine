#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class Texture;
	class EditorAssetPreviewPanel : public EditorPanel
	{
	public:
		EditorAssetPreviewPanel();
		~EditorAssetPreviewPanel();

		virtual void DrawPanel();

	private:
		Texture* mLoadingTexture = nullptr;
	};
}