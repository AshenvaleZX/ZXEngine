#pragma once
#include "EditorGUIManager.h"
#include "../Metal/MetalEnumStruct.h"

namespace ZXEngine
{
	class EditorGUIManagerMetal : public EditorGUIManager
	{
	public:
		EditorGUIManagerMetal();
		~EditorGUIManagerMetal();

		virtual void Init();
		virtual void ResetPanels();
		virtual void OnWindowSizeChange();

	protected:
		virtual void BeginEditorRender();
		virtual void EditorRender();
		virtual void EndEditorRender();

	private:
		vector<EditorPanel*> mAllPanels;

		MTL::RenderPassDescriptor* mRenderPassDescriptor = nullptr;

		void InitForMetal();
	};
}