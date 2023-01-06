#include "EditorConsolePanel.h"
#include "EditorDataManager.h"
#include "../Texture.h"
#include "../Resources.h"

namespace ZXEngine
{
	EditorConsolePanel::EditorConsolePanel()
	{
		logIcons[(int)LogType::Message] = new Texture(Resources::GetAssetFullPath("Textures/icons/message.png", true).c_str());
		logIcons[(int)LogType::Warning] = new Texture(Resources::GetAssetFullPath("Textures/icons/warning.png", true).c_str());
		logIcons[(int)LogType::Error] = new Texture(Resources::GetAssetFullPath("Textures/icons/error.png", true).c_str());
	}

	void EditorConsolePanel::DrawPanel()
	{
		// ����С��λ��
		ImGui::SetNextWindowPos(ImVec2((float)ProjectSetting::projectWidth, (float)ProjectSetting::mainBarHeight + (float)ProjectSetting::hierarchyHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::consoleWidth, (float)ProjectSetting::consoleHeight));

		// ��������������
		ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		{
			auto log = EditorDataManager::GetInstance()->logHead;
			while (true)
			{
				auto texID = logIcons[(int)log->type]->GetID();
				ImGui::Image((void*)(intptr_t)texID, iconSize);
				ImGui::SameLine();
				ImGui::Text(log->data.c_str());

				log = log->next;
				if (log == nullptr)
					break;
				else
					ImGui::Separator();
			}
		}
		ImGui::End();
	}
}