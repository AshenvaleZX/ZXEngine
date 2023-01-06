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
		// 面板大小和位置
		ImGui::SetNextWindowPos(ImVec2((float)ProjectSetting::projectWidth, (float)ProjectSetting::mainBarHeight + (float)ProjectSetting::hierarchyHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::consoleWidth, (float)ProjectSetting::consoleHeight));

		// 设置面板具体内容
		ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		{
			auto EditorData = EditorDataManager::GetInstance();
			
			ImGui::SetCursorPosX((float)(ProjectSetting::consoleWidth - 240));
			ImGui::Checkbox("##ShowMessage", &showMessage);
			ImGui::SameLine(); ImGui::Image((void*)(intptr_t)logIcons[0]->GetID(), iconSize);
			ImGui::SameLine(); ImGui::Text(to_string(EditorData->messageSize).c_str());
			ImGui::SameLine(); ImGui::Checkbox("##ShowWarning", &showWarning);
			ImGui::SameLine(); ImGui::Image((void*)(intptr_t)logIcons[1]->GetID(), iconSize);
			ImGui::SameLine(); ImGui::Text(to_string(EditorData->warningSize).c_str());
			ImGui::SameLine(); ImGui::Checkbox("##ShowError", &showError);
			ImGui::SameLine(); ImGui::Image((void*)(intptr_t)logIcons[2]->GetID(), iconSize);
			ImGui::SameLine(); ImGui::Text(to_string(EditorData->errorSize).c_str());

			const ImGuiID childId = ImGui::GetID((void*)(intptr_t)0);
			const bool childVisible = ImGui::BeginChild(childId, ImVec2((float)(ProjectSetting::consoleWidth - 10), (float)(ProjectSetting::consoleHeight - 60)));
			auto log = EditorData->logHead;
			bool firstLine = true;
			while (true)
			{
				if (log->type == LogType::Message && showMessage 
					|| log->type == LogType::Warning && showWarning 
					|| log->type == LogType::Error && showError)
				{
					if (firstLine)
						firstLine = false;
					else
						ImGui::Separator();
					auto texID = logIcons[(int)log->type]->GetID();
					ImGui::Image((void*)(intptr_t)texID, iconSize);
					ImGui::SameLine();
					ImGui::Text(log->data.c_str());
				}

				log = log->next;
				if (log == nullptr)
					break;
			}
			// 默认拉到最底部
			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
			ImGui::EndChild();
		}
		ImGui::End();
	}
}