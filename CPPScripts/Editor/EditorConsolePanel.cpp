#include "EditorConsolePanel.h"
#include "EditorDataManager.h"
#include "../Resources.h"

namespace ZXEngine
{
	EditorConsolePanel::EditorConsolePanel()
	{
		auto ImTextureMgr = ImGuiTextureManager::GetInstance();
		logIcons[(int)LogType::Message] = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/message.png", true));
		logIcons[(int)LogType::Warning] = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/warning.png", true));
		logIcons[(int)LogType::Error]   = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/error.png", true));
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
			ImGui::SameLine(); ImGui::Image(logIcons[0].ImGuiID, iconSize);
			ImGui::SameLine(); ImGui::Text("%d", EditorData->messageSize);
			ImGui::SameLine(); ImGui::Checkbox("##ShowWarning", &showWarning);
			ImGui::SameLine(); ImGui::Image(logIcons[1].ImGuiID, iconSize);
			ImGui::SameLine(); ImGui::Text("%d", EditorData->warningSize);
			ImGui::SameLine(); ImGui::Checkbox("##ShowError", &showError);
			ImGui::SameLine(); ImGui::Image(logIcons[2].ImGuiID, iconSize);
			ImGui::SameLine(); ImGui::Text("%d", EditorData->errorSize);

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
					ImGui::Image(logIcons[(int)log->type].ImGuiID, iconSize);
					ImGui::SameLine();
					ImGui::Text("%s", log->data.c_str());
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