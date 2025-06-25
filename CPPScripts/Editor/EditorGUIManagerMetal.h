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

		virtual void OnWindowSizeChange();

	protected:
		virtual void BeginEditorRender();
		virtual void EditorRender();
		virtual void EndEditorRender();

	private:
		MTL::RenderPassDescriptor* mRenderPassDescriptor = nullptr;

		void InitForMetal();
	};
}