#include "EditorInputManager.h"
#include "EditorEnumStruct.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ProjectSetting.h"
#include "../GlobalData.h"

namespace ZXEngine
{
	EditorInputManager* EditorInputManager::mInstance = nullptr;

	void EditorInputManager::Create()
	{
		mInstance = new EditorInputManager();
	}

	EditorInputManager* EditorInputManager::GetInstance()
	{
		return mInstance;
	}

	EditorAreaType EditorInputManager::CheckCurMousePos()
	{
		ImGuiIO& io = ImGui::GetIO();
		float posX = io.MousePos.x;
		float posY = io.MousePos.y;
		posX = Math::Max(0.0f, posX);
		posY = Math::Max(0.0f, posY);
		if (posX > ProjectSetting::hierarchyWidth && posX < ProjectSetting::hierarchyWidth + GlobalData::srcWidth
			&& posY > ProjectSetting::mainBarHeight && posY < ProjectSetting::mainBarHeight + GlobalData::srcHeight)
			return EditorAreaType::EAT_Game;
		else if (posX > ProjectSetting::projectWidth && posY > ProjectSetting::mainBarHeight + ProjectSetting::inspectorHeight)
			return EditorAreaType::EAT_AssetPreview;
		else
			return EditorAreaType::EAT_None;
	}
}