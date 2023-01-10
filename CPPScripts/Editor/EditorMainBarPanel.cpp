#include "EditorMainBarPanel.h"
#include "EditorDataManager.h"
#include "../GameLogicManager.h"
#include "../Time.h"
#include "../SceneManager.h"

namespace ZXEngine
{
	EditorMainBarPanel::EditorMainBarPanel()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4 btnColor = style.Colors[ImGuiCol_Button];
		selectBtnColor = ImVec4(btnColor.x - 0.1f, btnColor.y - 0.1f, btnColor.z - 0.1f, 1.0f);
		selectTextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void EditorMainBarPanel::DrawPanel()
	{
		// ����С��λ��
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::mainBarWidth, (float)ProjectSetting::mainBarHeight));

		// ��������������
		if (ImGui::Begin("ZXEngine", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Scene"))
						Debug::Log("Click New Scene");
					if (ImGui::MenuItem("Open Scene"))
						Debug::Log("Click Open Scene");
					if (ImGui::MenuItem("Save Scene"))
						Debug::Log("Click Save Scene");

					ImGui::Separator();

					if (ImGui::MenuItem("New Project"))
						Debug::Log("Click New Project");
					if (ImGui::MenuItem("Open Project"))
						Debug::Log("Click Open Project");
					if (ImGui::MenuItem("Save Project"))
						Debug::Log("Click Save Project");

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Project Settings"))
						Debug::Log("Click Project Settings");

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// 3����ť�����ҵĿ��
			float threeButtonSize = 80.0f;
			float avail = ImGui::GetContentRegionAvail().x;
			float offset = (avail - threeButtonSize) * 0.5f;
			// ����3����ťҪ���еĻ�����һ����ť����ʼλ��
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

			bool isPop = false;
			// Play
			if (EditorDataManager::isGameStart)
			{
				isPop = true;
				ImGui::PushStyleColor(ImGuiCol_Text, selectTextColor);
				ImGui::PushStyleColor(ImGuiCol_Button, selectBtnColor);
			}
			if (ImGui::Button(">>", buttonSize))
			{
				// �ص���Ϸ���е�ʱ��������ͣ��ť
				if (EditorDataManager::isGameStart)
				{
					Time::curTime = 0.0f;
					EditorDataManager::isGamePause = false;
					SceneManager::GetInstance()->ReloadScene();
				}
				EditorDataManager::isGameStart = !EditorDataManager::isGameStart;
			}
			if (isPop)
			{
				isPop = false;
				ImGui::PopStyleColor(2);
			}

			// Pause
			if (EditorDataManager::isGamePause)
			{
				isPop = true;
				ImGui::PushStyleColor(ImGuiCol_Text, selectTextColor);
				ImGui::PushStyleColor(ImGuiCol_Button, selectBtnColor);
			}
			ImGui::SameLine();
			if (ImGui::Button("||", buttonSize))
			{
				EditorDataManager::isGamePause = !EditorDataManager::isGamePause;
			}
			if (isPop)
			{
				isPop = false;
				ImGui::PopStyleColor(2);
			}

			// Step
			ImGui::SameLine();
			if (ImGui::Button(">|", buttonSize))
			{
				if (EditorDataManager::isGameStart && EditorDataManager::isGamePause)
				{
					Time::UpdateCurTime();
					GameLogicManager::GetInstance()->Update();
				}
			}
		}
		ImGui::End();
	}
}