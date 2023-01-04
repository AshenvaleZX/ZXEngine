#include "EditorMainBarPanel.h"

namespace ZXEngine
{
	void EditorMainBarPanel::DrawPanel()
	{
		// 面板大小和位置
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::mainBarWidth, (float)ProjectSetting::mainBarHeight));

		// 设置面板具体内容
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

			// 3个按钮从左到右的宽度
			float threeButtonSize = 80.0f;
			float avail = ImGui::GetContentRegionAvail().x;
			float offset = (avail - threeButtonSize) * 0.5f;
			// 计算3个按钮要居中的话，第一个按钮的起始位置
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

			ImVec2 buttonSize = ImVec2(22.0f, 22.0f);
			if (ImGui::Button(">>", buttonSize))
				Debug::Log("Click Begin");

			ImGui::SameLine();
			if (ImGui::Button("||", buttonSize))
				Debug::Log("Click Pause");

			ImGui::SameLine();
			if (ImGui::Button(">|", buttonSize))
				Debug::Log("Click Continue");
		}
		ImGui::End();
	}
}