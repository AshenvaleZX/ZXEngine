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
			// Preview����������������༭������ק״̬
			if (ImGui::IsMouseDown(0))
				isDragging = true;
		}

		if (isDragging)
		{
			ImGuiIO& io = ImGui::GetIO();
			// ��ק״̬����תԤ��ģ��
			EditorGUIManager::GetInstance()->assetPreviewer->UpdateModelRotation(io.MouseDelta.x, io.MouseDelta.y);

			// ��ק״̬���ɿ��������˳���ק״̬
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
		if (posX > ProjectSetting::hierarchyWidth && posX < ProjectSetting::hierarchyWidth + GlobalData::srcWidth
			&& posY > ProjectSetting::mainBarHeight && posY < ProjectSetting::mainBarHeight + GlobalData::srcHeight)
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
		// ��Ϸ�ڵļ�������Ĭ�϶�Ҫ����
		return true;
	}

	bool EditorInputManager::IsProcessGameMouseInput()
	{
		// �����ǰδ���ڱ༭����ק״̬��������걻��Ϸ������ߵ�ǰλ������Ϸ���������ڣ�����Ҫ������Ϸ���������
		return !isDragging && (!InputManager::GetInstance()->IsShowCursor() || CheckCurMousePos() == EditorAreaType::Game);
	}

	void EditorInputManager::UpdateMouseScroll(float xOffset, float yOffset)
	{
		// �����༭������������ͳһ��ImGuiIO�ģ�����ImGuiIO��MouseWheel����Ҫ��ImGui::Begin��End֮����ò�����Ч
		if (CheckCurMousePos() == EditorAreaType::AssetPreview)
			EditorGUIManager::GetInstance()->assetPreviewer->UpdateModelScale(yOffset);
	}
}