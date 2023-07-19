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
		EditorPanel() {};
		~EditorPanel() {};

		virtual void DrawPanel() = 0;
		virtual void ResetPanel() {};
	};
}