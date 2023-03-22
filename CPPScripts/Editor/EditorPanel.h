#pragma once
#include "../pubh.h"
#include "../GlobalData.h"
#include "../ProjectSetting.h"

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#ifdef ZX_API_OPENGL
#include "../ImGui/imgui_impl_opengl3.h"
#endif
#ifdef ZX_API_VULKAN
#include "../ImGui/imgui_impl_vulkan.h"
#endif

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