#include "EditorInputManager.h"
#include "EditorGUIManager.h"
#include "EditorAssetPreviewer.h"
#include "../External/ImGui/imgui.h"
#include "../External/ImGui/imgui_internal.h"
#include "../External/ImGui/imgui_impl_glfw.h"
#include "../External/ImGui/imgui_impl_opengl3.h"
#include "../ProjectSetting.h"
#include "../GlobalData.h"
#include "../Input/InputManager.h"

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

	void EditorInputManager::Update()
	{
		if (IsProcessGameMouseInput())
			return;


		if (CheckCurMousePos() == EditorAreaType::AssetPreview)
		{
			// Preview区域按下鼠标左键进入编辑器的拖拽状态
			if (ImGui::IsMouseDown(0))
				isDragging = true;
		}

		if (isDragging)
		{
			ImGuiIO& io = ImGui::GetIO();
			// 拖拽状态下旋转预览模型
			EditorGUIManager::GetInstance()->assetPreviewer->UpdateModelRotation(io.MouseDelta.x, io.MouseDelta.y);

			// 拖拽状态下松开鼠标左键退出拖拽状态
			if (!ImGui::IsMouseDown(0))
				isDragging = false;
		}
	}

	EditorAreaType EditorInputManager::CheckCurMousePos()
	{
		ImGuiIO& io = ImGui::GetIO();
		float posX = io.MousePos.x;
		float posY = io.MousePos.y;
		posX = Math::Max(0.0f, posX);
		posY = Math::Max(0.0f, posY);

		float headerSize = EditorGUIManager::GetInstance()->mHeaderSize;
		const Vector2& viewBorderSize = EditorGUIManager::GetInstance()->mViewBorderSize;

		if (posX > (ProjectSetting::hierarchyWidth + viewBorderSize.x) &&
			posX < (ProjectSetting::hierarchyWidth + viewBorderSize.x + GlobalData::srcWidth) &&
			posY > (ProjectSetting::mainBarHeight + viewBorderSize.y + headerSize) &&
			posY < (ProjectSetting::mainBarHeight + viewBorderSize.y + headerSize + GlobalData::srcHeight))
			return EditorAreaType::Game;
		else if (posX > ProjectSetting::srcWidth - ProjectSetting::inspectorWidth && posY > ProjectSetting::mainBarHeight + ProjectSetting::inspectorHeight)
			return EditorAreaType::AssetPreview;
		else
			return EditorAreaType::None;
	}

	bool EditorInputManager::IsProcessGameInput()
	{
		return IsProcessGameKeyInput() || IsProcessGameMouseInput();
	}

	bool EditorInputManager::IsProcessGameKeyInput()
	{
		// 游戏内的键盘输入默认都要处理
		return true;
	}

	bool EditorInputManager::IsProcessGameMouseInput()
	{
		// 如果当前未处于编辑器拖拽状态，并且鼠标被游戏捕获或者当前位置在游戏画面区域内，则需要处理游戏的鼠标输入
		return !isDragging && (!InputManager::GetInstance()->IsShowCursor() || CheckCurMousePos() == EditorAreaType::Game);
	}

	void EditorInputManager::UpdateMouseScroll(float xOffset, float yOffset)
	{
		// 本来编辑器的输入是想统一用ImGuiIO的，但是ImGuiIO的MouseWheel必须要在ImGui::Begin和End之间调用才能生效
		if (CheckCurMousePos() == EditorAreaType::AssetPreview)
			EditorGUIManager::GetInstance()->assetPreviewer->UpdateModelScale(yOffset);
	}
}