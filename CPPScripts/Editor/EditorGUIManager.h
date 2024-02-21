#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class EditorAssetPreviewer;
	class EditorGUIManager
	{
	public:
		static void Create();
		static EditorGUIManager* GetInstance();

	private:
		static EditorGUIManager* mInstance;

	public:
		EditorAssetPreviewer* assetPreviewer;

		void Render();

		virtual void Init()               = 0;
		virtual void ResetPanels()        = 0;
		virtual void OnWindowSizeChange() = 0;

	protected:
		virtual void BeginEditorRender() = 0;
		virtual void EditorRender()      = 0;
		virtual void EndEditorRender()   = 0;
	};
}