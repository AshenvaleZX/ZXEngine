#pragma once
#include "EditorPanel.h"
#include "EditorEnumStruct.h"

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
		Vector2 mViewBorderSize;
		float mHeaderSize = 0.0f;
		EditorAssetPreviewer* assetPreviewer = nullptr;

		void Render();

		void Init();
		void ResetPanels();
		EditorPanel* GetPanel(EditorPanelType type);

		virtual void OnWindowSizeChange() = 0;

	protected:
		vector<EditorPanel*> mAllPanels;

		virtual void BeginEditorRender() = 0;
		virtual void EditorRender()      = 0;
		virtual void EndEditorRender()   = 0;
	};
}