#pragma once
#include "../pubh.h"
#include "../GlobalData.h"
#include "../ProjectSetting.h"
#include "../External/ImGui/imgui.h"

namespace ZXEngine
{
	class EditorPanel
	{
	public:
		EditorPanel() = default;
		~EditorPanel() = default;

		virtual void DrawPanel() = 0;
		virtual void ResetPanel() {};

	protected:
		void CheckManualResize();
		virtual void OnPanelSizeChange(const Vector2& size) {};

	private:
		Vector2 mSize;
		Vector2 mDragSize;
		bool mPressing = false;
		bool mResizing = false;
	};
}