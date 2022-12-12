#pragma once
#include "../pubh.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ProjectSetting.h"
#include "../GlobalData.h"

namespace ZXEngine
{
	class EditorPanel
	{
	public:
		EditorPanel() {};
		~EditorPanel() {};

		virtual void DrawPanel() = 0;
	};
}